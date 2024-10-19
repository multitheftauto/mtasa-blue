/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * Copyright 2022 Craig Edwards and D++ contributors
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

#include <dpp/utility.h>

namespace dpp {

struct task_dummy {
	int* handle_dummy = nullptr;
};

}

#ifdef DPP_CORO

#include "coro.h"

#include <utility>
#include <type_traits>
#include <optional>
#include <functional>
#include <mutex>
#include <exception>
#include <atomic>

#include <iostream> // std::cerr in final_suspend

namespace dpp {

namespace detail {

/* Internal cogwheels for dpp::task */
namespace task {

/**
 * @brief State of a task
 */
enum class state_t {
	/**
	 * @brief Task was started but never co_await-ed
	 */
	started,
	/**
	 * @brief Task was co_await-ed and is pending completion
	 */
	awaited,
	/**
	 * @brief Task is completed
	 */
	done,
	/**
	 * @brief Task is still running but the actual dpp::task object is destroyed
	 */
	dangling
};

/**
 * @brief A @ref dpp::task "task"'s promise_t type, with special logic for handling nested tasks.
 *
 * @tparam R Return type of the task
 */
template <typename R>
struct promise_t;

/**
 * @brief The object automatically co_await-ed at the end of a @ref dpp::task "task". Ensures nested coroutine chains are resolved, and the promise_t cleans up if it needs to.
 *
 * @tparam R Return type of the task
 */
template <typename R>
struct final_awaiter;

/**
 * @brief Alias for <a href="https://en.cppreference.com/w/cpp/coroutine/coroutine_handle"std::coroutine_handle</a> for a @ref dpp::task "task"'s @ref promise_t.
 *
 * @tparam R Return type of the task
 */
template <typename R>
using handle_t = std_coroutine::coroutine_handle<promise_t<R>>;

/**
 * @brief Base class of @ref dpp::task.
 *
 * @warning This class should not be used directly by a user, use @ref dpp::task instead.
 * @note This class contains all the functions used internally by co_await. It is intentionally opaque and a private base of @ref dpp::task so a user cannot call await_suspend() and await_resume() directly.
 */
template <typename R>
class task_base {
protected:
	/**
	 * @brief The coroutine handle of this task.
	 */
	handle_t<R> handle;

	/**
	 * @brief Promise type of this coroutine. For internal use only, do not use.
	 */
	friend struct promise_t<R>;

private:
	/**
	 * @brief Construct from a coroutine handle. Internal use only
	 */
	explicit task_base(handle_t<R> handle_) : handle(handle_) {}

public:
	/**
	 * @brief Default constructor, creates a task not bound to a coroutine.
	 */
	task_base() = default;

	/**
	 * @brief Copy constructor is disabled
	 */
	task_base(const task_base &) = delete;

	/**
	 * @brief Move constructor, grabs another task's coroutine handle
	 *
	 * @param other Task to move the handle from
	 */
	task_base(task_base &&other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

	/**
	 * @brief Destructor.
	 *
	 * Destroys the handle.
	 * @warning The coroutine must be finished before this is called, otherwise it runs the risk of being resumed after it is destroyed, resuming in use-after-free undefined behavior.
	 */
	~task_base() {
		if (handle) {
			promise_t<R> &promise = handle.promise();
			state_t previous_state = promise.state.exchange(state_t::dangling);

			if (previous_state == state_t::done) {
				handle.destroy();
			}
			else {
				cancel();
			}
		}
	}

	/**
	 * @brief Copy assignment is disabled
	 */
	task_base &operator=(const task_base &) = delete;

	/**
	 * @brief Move assignment, grabs another task's coroutine handle
	 *
	 * @param other Task to move the handle from
	 */
	task_base &operator=(task_base &&other) noexcept {
		handle = std::exchange(other.handle, nullptr);
		return (*this);
	}

	/**
	 * @brief Check whether or not a call to co_await will suspend the caller.
	 *
	 * This function is called by the standard library as a first step when using co_await. If it returns true then the caller is not suspended.
	 * @throws logic_exception if the task is empty.
	 * @return bool Whether not to suspend the caller or not
	 */
	[[nodiscard]] bool await_ready() const {
		if (!handle) {
			throw dpp::logic_exception{"cannot co_await an empty task"};
		}
		return handle.promise().state.load() == state_t::done;
	}

	/**
	 * @brief Second function called by the standard library when the task is co_await-ed, if await_ready returned false.
	 *
	 * Stores the calling coroutine in the promise to resume when this task suspends.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @param caller The calling coroutine, now suspended
	 * @return bool Whether to suspend the caller or not
	 */
	[[nodiscard]] bool await_suspend(std_coroutine::coroutine_handle<> caller) noexcept {
		promise_t<R> &my_promise = handle.promise();
		auto previous_state = state_t::started;

		my_promise.parent = caller;
		// Replace `sent` state with `awaited` ; if that fails, the only logical option is the state was `done`, in which case return false to resume
		if (!handle.promise().state.compare_exchange_strong(previous_state, state_t::awaited) && previous_state == state_t::done) {
			return false;
		}
		return true;
	}

	/**
	 * @brief Function to check if the task has finished its execution entirely
	 *
	 * @return bool Whether the task is finished.
	 */
	[[nodiscard]] bool done() const noexcept {
		return handle && handle.promise().state.load(std::memory_order_relaxed) == state_t::done;
	}

	/**
	 * @brief Cancel the task, it will stop the next time it uses co_await. On co_await-ing this task, throws dpp::task_cancelled_exception.
	 *
	 * @return *this
	 */
	dpp::task<R>& cancel() & noexcept {
		handle.promise().cancelled.exchange(true, std::memory_order_relaxed);
		return static_cast<dpp::task<R> &>(*this);
	}

	/**
	 * @brief Cancel the task, it will stop the next time it uses co_await. On co_await-ing this task, throws dpp::task_cancelled_exception.
	 *
	 * @return *this
	 */
	dpp::task<R>&& cancel() && noexcept {
		handle.promise().cancelled.exchange(true, std::memory_order_relaxed);
		return static_cast<dpp::task<R> &&>(*this);
	}

	/**
	 * @brief Function called by the standard library when resuming.
	 *
	 * @return Return value of the coroutine, handed to the caller of co_await.
	 */
	decltype(auto) await_resume() & {
		return static_cast<dpp::task<R> &>(*this).await_resume_impl();
	}

	/**
	 * @brief Function called by the standard library when resuming.
	 *
	 * @return Return value of the coroutine, handed to the caller of co_await.
	 */
	decltype(auto) await_resume() const & {
		return static_cast<const dpp::task<R> &>(*this).await_resume_impl();
	}

	/**
	 * @brief Function called by the standard library when resuming.
	 *
	 * @return Return value of the coroutine, handed to the caller of co_await.
	 */
	decltype(auto) await_resume() && {
		return static_cast<dpp::task<R> &&>(*this).await_resume_impl();
	}
};

} // namespace task

} // namespace detail

/**
 * @class task task.h coro/task.h
 * @brief A coroutine task. It starts immediately on construction and can be co_await-ed, making it perfect for parallel coroutines returning a value.
 *
 * @warning - This feature is EXPERIMENTAL. The API may change at any time and there may be bugs.
 * Please report any to <a href="https://github.com/brainboxdotcc/DPP/issues">GitHub Issues</a> or to our <a href="https://discord.gg/dpp">Discord Server</a>.
 * @tparam R Return type of the task. Cannot be a reference but can be void.
 */
template <typename R>
#ifndef _DOXYGEN_
requires (!std::is_reference_v<R>)
#endif
class task : private detail::task::task_base<R> {
	/**
	 * @brief Internal use only base class containing common logic between task<R> and task<void>. It also serves to prevent await_suspend and await_resume from being used directly.
	 *
	 * @warning For internal use only, do not use.
	 * @see operator co_await()
	 */
	friend class detail::task::task_base<R>;

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 * @return The result of the coroutine. This is returned to the awaiter as the result of co_await
	 */
	R& await_resume_impl() & {
		detail::task::promise_t<R> &promise = this->handle.promise();
		if (promise.exception) {
			std::rethrow_exception(promise.exception);
		}
		return *reinterpret_cast<R *>(promise.result_storage.data());
	}

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 * @return The result of the coroutine. This is returned to the awaiter as the result of co_await
	 */
	const R& await_resume_impl() const & {
		detail::task::promise_t<R> &promise = this->handle.promise();
		if (promise.exception) {
			std::rethrow_exception(promise.exception);
		}
		return *reinterpret_cast<R *>(promise.result_storage.data());
	}

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 * @return The result of the coroutine. This is returned to the awaiter as the result of co_await
	 */
	R&& await_resume_impl() && {
		detail::task::promise_t<R> &promise = this->handle.promise();
		if (promise.exception) {
			std::rethrow_exception(promise.exception);
		}
		return *reinterpret_cast<R *>(promise.result_storage.data());
	}

public:
#ifdef _DOXYGEN_ // :)
	/**
	 * @brief Default constructor, creates a task not bound to a coroutine.
	 */
	task() = default;

	/**
	 * @brief Copy constructor is disabled
	 */
	task(const task &) = delete;

	/**
	 * @brief Move constructor, grabs another task's coroutine handle
	 *
	 * @param other Task to move the handle from
	 */
	task(task &&other) noexcept;

	/**
	 * @brief Destructor.
	 *
	 * Destroys the handle.
	 * @warning The coroutine must be finished before this is called, otherwise it runs the risk of being resumed after it is destroyed, resuming in use-after-free undefined behavior.
	 */
	~task();

	/**
	 * @brief Copy assignment is disabled
	 */
	task &operator=(const task &) = delete;

	/**
	 * @brief Move assignment, grabs another task's coroutine handle
	 *
	 * @param other Task to move the handle from
	 */
	task &operator=(task &&other) noexcept;

	/**
	 * @brief Function to check if the task has finished its execution entirely
	 *
	 * @return bool Whether the task is finished.
	 */
	[[nodiscard]] bool done() const noexcept;

	/**
	 * @brief Cancel the task, it will stop the next time it uses co_await. On co_await-ing this task, throws dpp::task_cancelled_exception.
	 */
	dpp::task<R>& cancel() & noexcept;

	/**
	 * @brief Check whether or not a call to co_await will suspend the caller.
	 *
	 * This function is called by the standard library as a first step when using co_await. If it returns true then the caller is not suspended.
	 * @throws logic_exception if the task is empty.
	 * @return bool Whether not to suspend the caller or not
	 */
	[[nodiscard]] bool await_ready() const;
#else
	using detail::task::task_base<R>::task_base; // use task_base's constructors
	using detail::task::task_base<R>::operator=; // use task_base's assignment operators
	using detail::task::task_base<R>::done; // expose done() as public
	using detail::task::task_base<R>::cancel; // expose cancel() as public
	using detail::task::task_base<R>::await_ready; // expose await_ready as public
#endif

	/**
	 * @brief Suspend the current coroutine until the task completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as a reference.
	 */
	[[nodiscard]] auto& operator co_await() & noexcept {
		return static_cast<detail::task::task_base<R>&>(*this);
	}

	/**
	 * @brief Suspend the current coroutine until the task completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as a const reference.
	 */
	[[nodiscard]] const auto& operator co_await() const & noexcept {
		return static_cast<const detail::task::task_base<R>&>(*this);
	}

	/**
	 * @brief Suspend the current coroutine until the task completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as an rvalue reference.
	 */
	[[nodiscard]] auto&& operator co_await() && noexcept {
		return static_cast<detail::task::task_base<R>&&>(*this);
	}
};

#ifndef _DOXYGEN_ // don't generate this on doxygen because `using` doesn't work and 2 copies of coroutine_base's docs is enough
/**
 * @brief A coroutine task. It starts immediately on construction and can be co_await-ed, making it perfect for parallel coroutines returning a value.
 *
 * Can be used in conjunction with coroutine events via dpp::event_router_t::co_attach, or on its own.
 *
 * @warning - This feature is EXPERIMENTAL. The API may change at any time and there may be bugs. Please report any to <a href="https://github.com/brainboxdotcc/DPP/issues">GitHub issues</a> or to the <a href="https://discord.gg/dpp">D++ Discord server</a>.
 * @tparam R Return type of the coroutine. Cannot be a reference, can be void.
 */
template <>
class task<void> : private detail::task::task_base<void> {
	/**
	 * @brief Private base class containing common logic between task<R> and task<void>. It also serves to prevent await_suspend and await_resume from being used directly.
	 *
	 * @see operator co_await()
	 */
	friend class detail::task::task_base<void>;

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 */
	void await_resume_impl() const;

public:
	using detail::task::task_base<void>::task_base; // use task_base's constructors
	using detail::task::task_base<void>::operator=; // use task_base's assignment operators
	using detail::task::task_base<void>::done; // expose done() as public
	using detail::task::task_base<void>::cancel; // expose cancel() as public
	using detail::task::task_base<void>::await_ready; // expose await_ready as public

	/**
	 * @brief Suspend the current coroutine until the task completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, returns a reference to the contained result.
	 */
	auto& operator co_await() & {
		return static_cast<detail::task::task_base<void>&>(*this);
	}

	/**
	 * @brief Suspend the current coroutine until the task completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, returns a const reference to the contained result.
	 */
	const auto& operator co_await() const & {
		return static_cast<const detail::task::task_base<void>&>(*this);
	}

	/**
	 * @brief Suspend the current coroutine until the task completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, returns a reference to the contained result.
	 */
	auto&& operator co_await() && {
		return static_cast<detail::task::task_base<void>&&>(*this);
	}
};
#endif /* _DOXYGEN_ */

namespace detail::task {
/**
 * @brief Awaitable returned from task::promise_t's final_suspend. Resumes the parent and cleans up its handle if needed
 */
template <typename R>
struct final_awaiter {
	/**
	 * @brief Always suspend at the end of the task. This allows us to clean up and resume the parent
	 */
	[[nodiscard]] bool await_ready() const noexcept {
		return (false);
	}

	/**
	 * @brief The suspension logic of the coroutine when it finishes. Always suspend the caller, meaning cleaning up the handle is on us
	 *
	 * @param handle The handle of this coroutine
	 * @return std::coroutine_handle<> Handle to resume, which is either the parent if present or std::noop_coroutine() otherwise
	 */
	[[nodiscard]] std_coroutine::coroutine_handle<> await_suspend(handle_t<R> handle) const noexcept;

	/**
	 * @brief Function called when this object is co_awaited by the standard library at the end of final_suspend. Do nothing, return nothing
	 */
	void await_resume() const noexcept {}
};

/**
 * @brief Base implementation of task::promise_t, without the logic that would depend on the return type. Meant to be inherited from
 */
struct promise_base {
	/**
	 * @brief State of the task, used to keep track of lifetime and status
	 */
	std::atomic<state_t> state = state_t::started;

	/**
	 * @brief Whether the task is cancelled or not.
	 */
	std::atomic<bool> cancelled = false;

	/**
	 * @brief Parent coroutine to return to for nested coroutines.
	 */
	detail::std_coroutine::coroutine_handle<> parent = nullptr;

	/**
	 * @brief Exception ptr if any was thrown during the coroutine
	 *
	 * @see <a href="https://en.cppreference.com/w/cpp/error/exception_ptr>std::exception_ptr</a>
	 */
	std::exception_ptr exception = nullptr;

#ifdef DPP_CORO_TEST
	promise_base() {
		++coro_alloc_count<promise_base>;
	}

	~promise_base() {
		--coro_alloc_count<promise_base>;
	}
#endif

	/**
	 * @brief Function called by the standard library when the coroutine is created.
	 *
	 * @return <a href="https://en.cppreference.com/w/cpp/coroutine/suspend_never">std::suspend_never</a> Don't suspend, the coroutine starts immediately.
	 */
	[[nodiscard]] std_coroutine::suspend_never initial_suspend() const noexcept {
		return {};
	}

	/**
	 * @brief Function called by the standard library when an exception is thrown and not caught in the coroutine.
	 *
	 * Stores the exception pointer to rethrow on co_await. If the task object is destroyed and was not cancelled, throw instead
	 */
	void unhandled_exception() {
		exception = std::current_exception();
		if ((state.load() == task::state_t::dangling) && !cancelled) {
			throw;
		}
	}

	/**
	 * @brief Proxy awaitable that wraps any co_await inside the task and checks for cancellation on resumption
	 *
	 * @see await_transform
	 */
	template <typename A>
	struct proxy_awaiter {
		/** @brief The promise_t object bound to this proxy */
		const task::promise_base &promise;

		/** @brief The inner awaitable being awaited */
		A awaitable;

		/** @brief Wrapper for the awaitable's await_ready */
		[[nodiscard]] bool await_ready() noexcept(noexcept(awaitable.await_ready())) {
			return awaitable.await_ready();
		}

		/** @brief Wrapper for the awaitable's await_suspend */
		template <typename T>
		[[nodiscard]] decltype(auto) await_suspend(T&& handle) noexcept(noexcept(awaitable.await_suspend(std::forward<T>(handle)))) {
			return awaitable.await_suspend(std::forward<T>(handle));
		}

		/**
		 * @brief Wrapper for the awaitable's await_resume, throws if the task is cancelled
		 *
		 * @throw dpp::task_cancelled_exception If the task was cancelled
		 */
		decltype(auto) await_resume() {
			if (promise.cancelled.load()) {
				throw dpp::task_cancelled_exception{"task was cancelled"};
			}
			return awaitable.await_resume();
		}
	};

	/**
	 * @brief Function called whenever co_await is used inside of the task
	 *
	 * @throw dpp::task_cancelled_exception On resumption if the task was cancelled
	 *
	 * @return @ref proxy_awaiter Returns a proxy awaiter that will check for cancellation on resumption
	 */
	template <typename T>
	[[nodiscard]] auto await_transform(T&& expr) const noexcept(noexcept(co_await_resolve(std::forward<T>(expr)))) {
		using awaitable_t = decltype(co_await_resolve(std::forward<T>(expr)));
		return proxy_awaiter<awaitable_t>{*this, co_await_resolve(std::forward<T>(expr))};
	}
};

/**
 * @brief Implementation of task::promise_t for non-void return type
 */
template <typename R>
struct promise_t : promise_base {
	/**
	 * @brief Destructor. Destroys the value if it was constructed.
	 */
	~promise_t() {
		if (state.load() == state_t::done && !exception) {
			std::destroy_at(reinterpret_cast<R *>(result_storage.data()));
		}
	}

	/**
	 * @brief Stored return value of the coroutine.
	 *
	 */
	alignas(R) std::array<std::byte, sizeof(R)> result_storage;

	/**
	 * @brief Function called by the standard library when the coroutine co_returns a value.
	 *
	 * Stores the value internally to hand to the caller when it resumes.
	 *
	 * @param expr The value given to co_return
	 */
	void return_value(R&& expr) noexcept(std::is_nothrow_move_constructible_v<R>) requires std::move_constructible<R> {
		std::construct_at<R>(reinterpret_cast<R *>(result_storage.data()), static_cast<R&&>(expr));
	}

	/**
	 * @brief Function called by the standard library when the coroutine co_returns a value.
	 *
	 * Stores the value internally to hand to the caller when it resumes.
	 *
	 * @param expr The value given to co_return
	 */
	void return_value(const R &expr) noexcept(std::is_nothrow_copy_constructible_v<R>) requires std::copy_constructible<R> {
		std::construct_at<R>(reinterpret_cast<R *>(result_storage.data()), expr);
	}

	/**
	 * @brief Function called by the standard library when the coroutine co_returns a value.
	 *
	 * Stores the value internally to hand to the caller when it resumes.
	 *
	 * @param expr The value given to co_return
	 */
	template <typename T>
	requires (!std::is_same_v<R, std::remove_cvref_t<T>> && std::convertible_to<T, R>)
	void return_value(T&& expr) noexcept (std::is_nothrow_convertible_v<T, R>) {
		std::construct_at<R>(reinterpret_cast<R *>(result_storage.data()), std::forward<T>(expr));
	}

	/**
	 * @brief Function called by the standard library when the coroutine is created.
	 *
	 * @return dpp::task The coroutine object
	 */
	[[nodiscard]] dpp::task<R> get_return_object() noexcept {
		return dpp::task<R>{handle_t<R>::from_promise(*this)};
	}

	/**
	 * @brief Function called by the standard library when the coroutine reaches its last suspension point
	 *
	 * @return final_awaiter Special object containing the chain resolution and clean-up logic.
	 */
	[[nodiscard]] final_awaiter<R> final_suspend() const noexcept {
		return {};
	}
};

/**
 * @brief Implementation of task::promise_t for void return type
 */
template <>
struct promise_t<void> : promise_base {
	/**
	 * @brief Function called by the standard library when the coroutine co_returns
	 *
	 * Does nothing but is required by the standard library.
	 */
	void return_void() const noexcept {}

	/**
	 * @brief Function called by the standard library when the coroutine is created.
	 *
	 * @return task The coroutine object
	 */
	[[nodiscard]] dpp::task<void> get_return_object() noexcept {
		return dpp::task<void>{handle_t<void>::from_promise(*this)};
	}

	/**
	 * @brief Function called by the standard library when the coroutine reaches its last suspension point
	 *
	 * @return final_awaiter Special object containing the chain resolution and clean-up logic.
	 */
	[[nodiscard]] final_awaiter<void> final_suspend() const noexcept {
		return {};
	}
};

template <typename R>
std_coroutine::coroutine_handle<> final_awaiter<R>::await_suspend(handle_t<R> handle) const noexcept {
	promise_t<R> &promise = handle.promise();
	state_t previous_state = promise.state.exchange(state_t::done);

	switch (previous_state) {
		case state_t::started: // started but never awaited, suspend
			return std_coroutine::noop_coroutine();
		case state_t::awaited: // co_await-ed, resume parent
			return promise.parent;
		case state_t::dangling: // task object is gone, free the handle
			handle.destroy();
			return std_coroutine::noop_coroutine();
		case state_t::done: // what
			// this should never happen. log it. we don't have a cluster so just write it on cerr
			std::cerr << "dpp::task: final_suspend called twice. something went very wrong here, please report to GitHub issues or the D++ Discord server" << std::endl;
	}
	// TODO: replace with __builtin_unreachable when we confirm this never happens with normal usage
	return std_coroutine::noop_coroutine();
}

} // namespace detail::task

#ifndef _DOXYGEN_
inline void task<void>::await_resume_impl() const {
	if (handle.promise().exception) {
		std::rethrow_exception(handle.promise().exception);
	}
}
#endif /* _DOXYGEN_ */

DPP_CHECK_ABI_COMPAT(task<void>, task_dummy)
DPP_CHECK_ABI_COMPAT(task<uint64_t>, task_dummy)

} // namespace dpp

/**
 * @brief Specialization of std::coroutine_traits, helps the standard library figure out a promise_t type from a coroutine function.
 */
template<typename T, typename... Args>
struct dpp::detail::std_coroutine::coroutine_traits<dpp::task<T>, Args...> {
	using promise_type = dpp::detail::task::promise_t<T>;
};

#endif /* DPP_CORO */
