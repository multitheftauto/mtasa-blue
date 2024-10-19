/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * Copyright 2021 Craig Edwards and D++ contributors
 * (https://github.com/brainboxdotcc/DPP/graphs/contributors)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************************/

#pragma once

#include <dpp/export.h>
#include <string>
#include <map>
#include <variant>
#include <dpp/snowflake.h>
#include <dpp/misc-enum.h>
#include <dpp/json_fwd.h>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <cstring>
#include <atomic>
#include <dpp/exception.h>
#include <dpp/coro/job.h>
#include <dpp/coro/task.h>

namespace dpp {

#ifdef DPP_CORO

template <typename T>
class event_router_t;

namespace detail {

/** @brief Internal cogwheels for dpp::event_router_t */
namespace event_router {

/** @brief State of an owner of an event_router::awaitable */
enum class awaiter_state {
	/** @brief Awaitable is not being awaited */
	none,
	/** @brief Awaitable is being awaited */
	waiting,
	/** @brief Awaitable will be resumed imminently */
	resuming,
	/** @brief Awaitable will be cancelled imminently */
	cancelling
};

/**
 * @brief Awaitable object representing an event.
 * A user can co_await on this object to resume the next time the event is fired,
 * optionally with a condition.
 */
template <typename T>
class awaitable {
	friend class event_router_t<T>;

	/** @brief Resume the coroutine waiting on this object */
	void resume() {
		std_coroutine::coroutine_handle<>::from_address(handle).resume();
	}

	/** @brief Event router that will manage this object */
	event_router_t<T> *self;

	/** @brief Predicate on the event, or nullptr for always match */
	std::function<bool (const T &)> predicate = nullptr;

	/** @brief Event that triggered a resumption, to give to the resumer */
	const T *event = nullptr;

	/** @brief Coroutine handle, type-erased */
	void* handle = nullptr;

	/** @brief The state of the awaiting coroutine */
	std::atomic<awaiter_state> state = awaiter_state::none;

	/** Default constructor is accessible only to event_router_t */
	awaitable() = default;

	/** Normal constructor is accessible only to event_router_t */
	template <typename F>
	awaitable(event_router_t<T> *router, F&& fun) : self{router}, predicate{std::forward<F>(fun)} {}

public:
	/** This object is not copyable. */
	awaitable(const awaitable &) = delete;

	/** Move constructor. */
	awaitable(awaitable &&rhs) noexcept : self{rhs.self}, predicate{std::move(rhs.predicate)}, event{rhs.event}, handle{std::exchange(rhs.handle, nullptr)}, state{rhs.state.load(std::memory_order_relaxed)} {}

	/** This object is not copyable. */
	awaitable& operator=(const awaitable &) = delete;

	/** Move assignment operator. */
	awaitable& operator=(awaitable&& rhs) noexcept {
		self = rhs.self;
		predicate = std::move(rhs.predicate);
		event = rhs.event;
		handle = std::exchange(rhs.handle, nullptr);
		state = rhs.state.load(std::memory_order_relaxed);
		return *this;
	}

	/**
	 * @brief Request cancellation. This will detach this object from the event router and resume the awaiter, which will be thrown dpp::task_cancelled::exception.
	 *
	 * @throw ??? As this resumes the coroutine, it may throw any exceptions at the caller.
	 */
	void cancel();

	/**
	 * @brief First function called by the standard library when awaiting this object. Returns true if we need to suspend.
	 *
	 * @retval false always.
	 */
	[[nodiscard]] constexpr bool await_ready() const noexcept;

	/**
	 * @brief Second function called by the standard library when awaiting this object, after suspension.
	 * This will attach the object to its event router, to be resumed on the next event that satisfies the predicate.
	 *
	 * @return void never resume on call.
	 */
	void await_suspend(detail::std_coroutine::coroutine_handle<> caller);

	/**
	 * @brief Third and final function called by the standard library, called when resuming the coroutine.
	 *
	 * @throw @ref task_cancelled_exception if cancel() has been called
	 * @return const T& __Reference__ to the event that matched
	 */
	[[maybe_unused]] const T& await_resume();
};

}

}
#endif

/**
 * @brief A returned event handle for an event which was attached
 */
typedef size_t event_handle;

/**
 * @brief Handles routing of an event to multiple listeners.
 * Multiple listeners may attach to the event_router_t by means of @ref operator()(F&&) "operator()". Passing a
 * lambda into @ref operator()(F&&) "operator()" attaches to the event.
 * 
 * @details Dispatchers of the event may call the @ref call() method to cause all listeners
 * to receive the event.
 * 
 * The @ref empty() method will return true if there are no listeners attached
 * to the event_router_t (this can be used to save time by not constructing objects that
 * nobody will ever see).
 * 
 * The @ref detach() method removes an existing listener from the event,
 * using the event_handle ID returned by @ref operator()(F&&) "operator()".
 * 
 * This class is used by the library to route all websocket events to listening code.
 * 
 * Example:
 * 
 * @code{cpp}
 * // Declare an event that takes log_t as its parameter
 * event_router_t<log_t> my_event;
 * 
 * // Attach a listener to the event
 * event_handle id = my_event([&](const log_t& cc) {
 *	 std::cout << cc.message << "\n";
 * });
 * 
 * // Construct a log_t and call the event (listeners will receive the log_t object)
 * log_t lt;
 * lt.message = "foo";
 * my_event.call(lt);
 * 
 * // Detach from an event using the handle returned by operator()
 * my_event.detach(id);
 * @endcode
 * 
 * @tparam T type of single parameter passed to event lambda derived from event_dispatch_t
 */
template<class T> class event_router_t {
private:
	friend class cluster;

	/**
	 * @brief Non-coro event handler type
	 */
	using regular_handler_t = std::function<void(const T&)>;

	/**
	 * @brief Type that event handlers will be stored as with DPP_CORO off.
	 * This is the ABI DPP_CORO has to match.
	 */
	using event_handler_abi_t = std::variant<regular_handler_t, std::function<task_dummy(T)>>;

#ifdef DPP_CORO
	friend class detail::event_router::awaitable<T>;

	/** @brief dpp::task coro event handler */
	using task_handler_t = std::function<dpp::task<void>(const T&)>;

	/** @brief Type that event handlers are stored as */
	using event_handler_t = std::variant<regular_handler_t, task_handler_t>;

	DPP_CHECK_ABI_COMPAT(event_handler_t, event_handler_abi_t)
#else
	/**
	 * @brief Type that event handlers are stored as
	 */
	using event_handler_t = event_handler_abi_t;
#endif

	/**
	 * @brief Identifier for the next event handler, will be given to the user on attaching a handler
	 */
	event_handle next_handle = 1;

	/**
	 * @brief Thread safety mutex
	 */
	mutable std::shared_mutex mutex;

	/**
	 * @brief Container of event listeners keyed by handle,
	 * as handles are handed out sequentially they will always
	 * be called in they order they are bound to the event
	 * as std::map is an ordered container.
	 */
	std::map<event_handle, event_handler_t> dispatch_container;

#ifdef DPP_CORO
	/**
	 * @brief Mutex for messing with coro_awaiters.
	 */
	mutable std::shared_mutex coro_mutex;

	/**
	 * @brief Vector containing the awaitables currently being awaited on for this event router.
	 */
	mutable std::vector<detail::event_router::awaitable<T> *> coro_awaiters;
#else
	/**
	 * @brief Dummy for ABI compatibility between DPP_CORO and not
	 */
	utility::dummy<std::shared_mutex> definitely_not_a_mutex;

	/**
	 * @brief Dummy for ABI compatibility between DPP_CORO and not
	 */
	utility::dummy<std::vector<void*>> definitely_not_a_vector;
#endif

	/**
	 * @brief A function to be called whenever the method is called, to check
	 * some condition that is required for this event to trigger correctly.
	 */
	std::function<void(const T&)> warning;

	/**
	 * @brief Next handle to be given out by the event router
	 */

protected:

	/**
	 * @brief Set the warning callback object used to check that this
	 * event is capable of running properly
	 * 
	 * @param warning_function A checking function to call
	 */
	void set_warning_callback(std::function<void(const T&)> warning_function) {
		warning = warning_function;
	}

	/**
	 * @brief Handle an event. This function should only be used without coro enabled, otherwise use handle_coro.
	 */
	void handle(const T& event) const {
		if (warning) {
			warning(event);
		}

		std::shared_lock l(mutex);
		for (const auto& [_, listener] : dispatch_container) {
			if (!event.is_cancelled()) {
				if (std::holds_alternative<regular_handler_t>(listener)) {
					std::get<regular_handler_t>(listener)(event);
				} else {
					throw dpp::logic_exception("cannot handle a coroutine event handler with a library built without DPP_CORO");
				}
			}
		};
	}

#ifdef DPP_CORO
	/**
	 * @brief Handle an event as a coroutine, ensuring the lifetime of the event object.
	 */
	dpp::job handle_coro(T event) const {
		if (warning) {
			warning(event);
		}

		resume_awaiters(event);

		std::vector<dpp::task<void>> tasks;
		{
			std::shared_lock l(mutex);

			for (const auto& [_, listener] : dispatch_container) {
				if (!event.is_cancelled()) {
					if (std::holds_alternative<task_handler_t>(listener)) {
						tasks.push_back(std::get<task_handler_t>(listener)(event));
					} else if (std::holds_alternative<regular_handler_t>(listener)) {
						std::get<regular_handler_t>(listener)(event);
					}
				}
			};
		}

		for (dpp::task<void>& t : tasks) {
			co_await t; // keep the event object alive until all tasks finished
		}
	}

	/**
	 * @brief Attach a suspended coroutine to this event router via detail::event_router::awaitable.
	 * It will be resumed and detached when an event satisfying its condition completes, or it is cancelled.
	 *
	 * This is for internal usage only, the user way to do this is to co_await it (which will call this when suspending)
	 * This guarantees that the coroutine is indeed suspended and thus can be resumed at any time
	 *
	 * @param awaiter Awaiter to attach
	 */
	void attach_awaiter(detail::event_router::awaitable<T> *awaiter) {
		std::unique_lock lock{coro_mutex};

		coro_awaiters.emplace_back(awaiter);
	}

	/**
	 * @brief Detach an awaiting coroutine handle from this event router.
	 * This is mostly called when a detail::event_router::awaitable is cancelled.
	 *
	 * @param handle Coroutine handle to find in the attached coroutines
	 */
	void detach_coro(void *handle) {
		std::unique_lock lock{coro_mutex};

		coro_awaiters.erase(std::remove_if(coro_awaiters.begin(), coro_awaiters.end(), [handle](detail::event_router::awaitable<T> const *awaiter) { return awaiter->handle == handle; }), coro_awaiters.end());
	}

	/**
	 * @brief Resume any awaiter whose predicate matches this event, or is null.
	 *
	 * @param event Event to compare and pass to accepting awaiters
	 */
	void resume_awaiters(const T& event) const {
		std::vector<detail::event_router::awaitable<T>*> to_resume;
		std::unique_lock lock{coro_mutex};

		for (auto it = coro_awaiters.begin(); it != coro_awaiters.end();) {
			detail::event_router::awaitable<T>* awaiter = *it;

			if (awaiter->predicate && !awaiter->predicate(event)) {
				++it;
			} else {
				using state_t = detail::event_router::awaiter_state;

				/**
				 * If state == none (was never awaited), do nothing
				 * If state == waiting, prevent resumption, resume on our end
				 * If state == resuming || cancelling, ignore
				 *
				 * Technically only cancelling || waiting should be possible here
				 * We do this by trying to exchange "waiting" with "resuming". If that returns false, this is presumed to be "cancelling"
				 */

				state_t s = state_t::waiting;
				if (awaiter->state.compare_exchange_strong(s, state_t::resuming)) {
					to_resume.emplace_back(awaiter);
					awaiter->event = &event;

					it = coro_awaiters.erase(it);
				} else {
					++it;
				}
			}
		}
		lock.unlock();
		for (detail::event_router::awaitable<T>* awaiter : to_resume)
			awaiter->resume();
	}
#endif

public:
	/**
	 * @brief Construct a new event_router_t object.
	 */
	event_router_t() = default;

	/**
	 * @brief Destructor. Will cancel any coroutine awaiting on events.
	 *
	 * @throw ! Cancelling a coroutine will throw a dpp::task_cancelled_exception to it.
	 * This will be caught in this destructor, however, make sure no other exceptions are thrown in the coroutine after that or it will terminate.
	 */
	~event_router_t() {
#ifdef DPP_CORO
		while (!coro_awaiters.empty()) {
			// cancel all awaiters. here we cannot do the usual loop as we'd need to lock coro_mutex, and cancel() locks and modifies coro_awaiters
			try {
				coro_awaiters.back()->cancel();
				 /*
					* will resume coroutines and may throw ANY exception, including dpp::task_cancelled_exception cancel() throws at them.
					* we catch that one. for the rest, good luck :)
					* realistically the only way any other exception would pop up here is if someone catches dpp::task_cancelled_exception THEN throws another exception.
				  */
			} catch (const dpp::task_cancelled_exception &) {
				// ok. likely we threw this one
			}
		}
#endif
	}

	/**
	 * @brief Call all attached listeners.
	 * Listeners may cancel, by calling the event.cancel method.
	 *
	 * @param event Class to pass as parameter to all listeners.
	 */
	void call(const T& event) const {
#ifdef DPP_CORO
		handle_coro(event);
#else
		handle(event);
#endif
	};

	/**
	 * @brief Call all attached listeners.
	 * Listeners may cancel, by calling the event.cancel method.
	 *
	 * @param event Class to pass as parameter to all listeners.
	 */
	void call(T&& event) const {
#ifdef DPP_CORO
		handle_coro(std::move(event));
#else
		handle(std::move(event));
#endif
	};

#ifdef DPP_CORO
	/**
	 * @brief Obtain an awaitable object that refers to an event with a certain condition.
	 * It can be co_await-ed to wait for the next event that satisfies this condition.
	 * On resumption the awaiter will be given __a reference__ to the event,
	 * saving it in a variable is recommended to avoid variable lifetime issues.
	 *
	 * @details Example: @code{cpp}
	 * dpp::job my_handler(dpp::slashcommand_t event) {
	 *	co_await event.co_reply(dpp::message().add_component(dpp::component().add_component().set_label("click me!").set_id("test")));
	 *
	 *	dpp::button_click_t b = co_await c->on_button_click.with([](const dpp::button_click_t &event){ return event.custom_id == "test"; });
	 *
	 *	// do something on button click
	 * }
	 * @endcode
	 *
	 * This can be combined with dpp::when_any and other awaitables, for example dpp::cluster::co_sleep to create @ref expiring-buttons "expiring buttons".
	 *
	 * @warning On resumption the awaiter will be given <b>a reference</b> to the event.
	 * This means that variable may become dangling at the next co_await, be careful and save it in a variable
	 * if you need to.
	 * @param pred Predicate to check the event against. This should be a callable of the form `bool(const T&)`
	 * where T is the event type, returning true if the event is to match.
	 * @return awaitable An awaitable object that can be co_await-ed to await an event matching the condition.
	 */
	template <typename Predicate>
#ifndef _DOXYGEN_
	requires utility::callable_returns<Predicate, bool, const T&>
#endif
	auto when(Predicate&& pred)
#ifndef _DOXYGEN_
		noexcept(noexcept(std::function<bool(const T&)>{std::declval<Predicate>()}))
#endif
	{
		return detail::event_router::awaitable<T>{this, std::forward<Predicate>(pred)};
	}

	/**
	 * @brief Obtain an awaitable object that refers to any event.
	 * It can be co_await-ed to wait for the next event.
	 *
	 * Example:
	 * @details Example: @code{cpp}
	 * dpp::job my_handler(dpp::slashcommand_t event) {
	 *	co_await event.co_reply(dpp::message().add_component(dpp::component().add_component().set_label("click me!").set_id("test")));
	 *
	 *	dpp::button_click_t b = co_await c->on_message_create;
	 *
	 *	// do something on button click
	 * }
	 * @endcode
	 *
	 * This can be combined with dpp::when_any and other awaitables, for example dpp::cluster::co_sleep to create expiring buttons.
	 *
	 * @warning On resumption the awaiter will be given <b>a reference</b> to the event.
	 * This means that variable may become dangling at the next co_await, be careful and save it in a variable
	 * if you need to.
	 * @return awaitable An awaitable object that can be co_await-ed to await an event matching the condition.
	 */
	[[nodiscard]] auto operator co_await() noexcept {
		return detail::event_router::awaitable<T>{this, nullptr};
	}
#endif

	/**
	 * @brief Returns true if the container of listeners is empty,
	 * i.e. there is nothing listening for this event right now.
	 * 
	 * @retval true  if there are no listeners
	 * @retval false if there are some listeners
	 */
	[[nodiscard]] bool empty() const {
#ifdef DPP_CORO
		std::shared_lock lock{mutex};
		std::shared_lock coro_lock{coro_mutex};

		return dispatch_container.empty() && coro_awaiters.empty();
#else
		std::shared_lock lock{mutex};

		return dispatch_container.empty();
#endif
	}

	/**
	 * @brief Returns true if any listeners are attached.
	 * 
	 * This is the boolean opposite of event_router_t::empty().
	 * @retval true  if listeners are attached
	 * @retval false if no listeners are attached
	 */
	operator bool() const {
		return !empty();
	}

#ifdef _DOXYGEN_
	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should either be of the form `void(const T&)` or
	 * `dpp::task<void>(const T&)` (the latter requires DPP_CORO to be defined),
	 * where T is the event type for this event router.
	 *
	 * This has the exact same behavior as using \ref attach(F&&) "attach".
	 *
	 * @see attach
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	[[maybe_unused]] event_handle operator()(F&& fun);

	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should either be of the form `void(const T&)` or
	 * `dpp::task<void>(const T&)` (the latter requires DPP_CORO to be defined),
	 * where T is the event type for this event router.
	 *
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	[[maybe_unused]] event_handle attach(F&& fun);
#else /* not _DOXYGEN_ */
#  ifdef DPP_CORO
	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should either be of the form `void(const T&)` or
	 * `dpp::task<void>(const T&)`, where T is the event type for this event router.
	 *
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	requires (utility::callable_returns<F, dpp::job, const T&> || utility::callable_returns<F, dpp::task<void>, const T&> || utility::callable_returns<F, void, const T&>)
	[[maybe_unused]] event_handle operator()(F&& fun) {
		return this->attach(std::forward<F>(fun));
	}

	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should either be of the form `void(const T&)` or
	 * `dpp::task<void>(const T&)`, where T is the event type for this event router.
	 *
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	requires (utility::callable_returns<F, void, const T&>)
	[[maybe_unused]] event_handle attach(F&& fun) {
		std::unique_lock l(mutex);
		event_handle h = next_handle++;
		dispatch_container.emplace(std::piecewise_construct, std::forward_as_tuple(h), std::forward_as_tuple(std::in_place_type_t<regular_handler_t>{}, std::forward<F>(fun)));
		return h;
	}

	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should either be of the form `void(const T&)` or
	 * `dpp::task<void>(const T&)`, where T is the event type for this event router.
	 *
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	requires (utility::callable_returns<F, task<void>, const T&>)
	[[maybe_unused]] event_handle attach(F&& fun) {
		assert(dpp::utility::is_coro_enabled());

		std::unique_lock l(mutex);
		event_handle h = next_handle++;
		dispatch_container.emplace(std::piecewise_construct, std::forward_as_tuple(h), std::forward_as_tuple(std::in_place_type_t<task_handler_t>{}, std::forward<F>(fun)));
		return h;
	}

	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should either be of the form `void(const T&)` or
	 * `dpp::task<void>(const T&)`, where T is the event type for this event router.
	 *
	 * @deprecated dpp::job event handlers are deprecated and will be removed in a future version, use dpp::task<void> instead.
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	requires (utility::callable_returns<F, dpp::job, const T&>)
	DPP_DEPRECATED("dpp::job event handlers are deprecated and will be removed in a future version, use dpp::task<void> instead")
	[[maybe_unused]] event_handle attach(F&& fun) {
		assert(dpp::utility::is_coro_enabled());

		std::unique_lock l(mutex);
		event_handle h = next_handle++;
		dispatch_container.emplace(std::piecewise_construct, std::forward_as_tuple(h), std::forward_as_tuple(std::in_place_type_t<regular_handler_t>{}, std::forward<F>(fun)));
		return h;
	}
#  else
	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should be of the form `void(const T&)`
	 * where T is the event type for this event router.
	 *
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	[[maybe_unused]] std::enable_if_t<utility::callable_returns_v<F, void, const T&>, event_handle> operator()(F&& fun) {
		return this->attach(std::forward<F>(fun));
	}

	/**
	 * @brief Attach a callable to the event, adding a listener.
	 * The callable should be of the form `void(const T&)`
	 * where T is the event type for this event router.
	 *f
	 * @warning You cannot call this within an event handler.
	 *
	 * @param fun Callable to attach to event
	 * @return event_handle An event handle unique to this event, used to
	 * detach the listener from the event later if necessary.
	 */
	template <typename F>
	[[maybe_unused]] std::enable_if_t<utility::callable_returns_v<F, void, const T&>, event_handle> attach(F&& fun) {
		std::unique_lock l(mutex);
		event_handle h = next_handle++;
		dispatch_container.emplace(h, std::forward<F>(fun));
		return h;
	}
#  endif /* DPP_CORO */
#endif /* _DOXYGEN_ */
	/**
	 * @brief Detach a listener from the event using a previously obtained ID.
	 *
	 * @warning You cannot call this within an event handler.
	 *
	 * @param handle An ID obtained from @ref operator(F&&) "operator()"
	 * @retval true  The event was successfully detached
	 * @retval false The ID is invalid (possibly already detached, or does not exist)
	 */
	[[maybe_unused]] bool detach(const event_handle& handle) {
		std::unique_lock l(mutex);
		return this->dispatch_container.erase(handle);
	}
};

#ifdef DPP_CORO

namespace detail::event_router {

template <typename T>
void awaitable<T>::cancel() {
	awaiter_state s = awaiter_state::waiting;
	/**
		* If state == none (was never awaited), do nothing
		* If state == waiting, prevent resumption, resume on our end
		* If state == resuming || cancelling, ignore
		*/
	if (state.compare_exchange_strong(s, awaiter_state::cancelling)) {
		self->detach_coro(handle);
		resume();
	}
}

template <typename T>
constexpr bool awaitable<T>::await_ready() const noexcept {
	return false;
}

template <typename T>
void awaitable<T>::await_suspend(detail::std_coroutine::coroutine_handle<> caller) {
	state.store(awaiter_state::waiting);
	handle = caller.address();
	self->attach_awaiter(this);
}

template <typename T>
const T &awaitable<T>::await_resume() {
	handle = nullptr;
	predicate = nullptr;
	if (state.exchange(awaiter_state::none, std::memory_order_relaxed) == awaiter_state::cancelling) {
		throw dpp::task_cancelled_exception{"event_router::awaitable was cancelled"};
	}
	return *std::exchange(event, nullptr);
}

}
#endif

} // namespace dpp
