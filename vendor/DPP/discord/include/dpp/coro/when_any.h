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

#ifdef DPP_CORO
#pragma once

#include "coro.h"
#include "job.h"

#include <atomic>
#include <array>
#include <memory>
#include <limits>
#include <optional>

namespace dpp {

template <typename T>
class event_router_t;

namespace detail {

namespace event_router {

template <typename T>
class awaitable;

}

/**
 * @brief Internal cogwheels for dpp::when_any
 */
namespace when_any {

/**
 * @brief Current state of a when_any object
 */
enum class await_state {
	/**
	 * @brief Object was started but not awaited
	 */
	started,
	/**
	 * @brief Object is being awaited
	 */
	waiting,
	/**
	 * @brief Object was resumed
	 */
	done,
	/**
	 * @brief Object was destroyed
	 */
	dangling
};

/**
 * @brief Type trait helper to obtain the actual type that will be used by a when_any when a type is passed as a parameter.
 * May specialize for certain types for specific behavior, e.g. for an event_router, store the awaitable directly
 */
template <typename T>
struct arg_helper_s {
	/** Raw type of the awaitable */
	using type = T;

	/** Helper static method to get the awaitable from a variable */
	static decltype(auto) get(auto&& v) {
		return static_cast<decltype(v)>(v);
	}
};

template <typename T>
struct arg_helper_s<dpp::event_router_t<T>> {
	using type = event_router::awaitable<T>;

	template <typename U>
#ifndef _DOXYGEN
	requires (std::same_as<std::remove_cvref_t<U>, dpp::event_router_t<T>>)
#endif
	static event_router::awaitable<T> get(U&& v) {
		return static_cast<U>(v).operator co_await();
	}
};

/**
 * @brief Alias for the actual type that an awaitable will be stored as in a when_any.
 * For example if given an event_router, store the awaitable, not the event_router.
 */
template <typename T>
using awaitable_type = typename arg_helper_s<T>::type;

/**
 * @brief Helper struct with a method to convert an awaitable parameter to the actual value it will be stored as.
 * For example if given an event_router, store the awaitable, not the event_router.
 */
template <typename T>
using arg_helper = arg_helper_s<std::remove_cvref_t<T>>;

/**
 * @brief Empty result from void-returning awaitable
 */
struct empty{};

/**
 * @brief Actual type a result will be stores as in when_any
 */
template <typename T>
using storage_type = std::conditional_t<std::is_void_v<T>, empty, T>;

/**
 * @brief Concept satisfied if a stored result is void
 */
template <typename T>
concept void_result = std::same_as<T, empty>;

}

} // namespace detail

/**
 * @class when_any when_any.h coro/when_any.h
 * @brief Experimental class to co_await on a bunch of awaitable objects, resuming when the first one completes.
 * On completion, returns a @ref result object that contains the index of the awaitable that finished first.
 * A user can call @ref result::index() and @ref result::get<N>() on the result object to get the result, similar to std::variant.
 *
 * @see when_any::result
 * @tparam Args... Type of each awaitable to await on
 */
template <typename... Args>
#ifndef _DOXYGEN_
requires (sizeof...(Args) >= 1)
#endif
class when_any {
	/**
	 * @brief Alias for the type of the result variant
	 */
	using variant_type = std::variant<std::exception_ptr, std::remove_cvref_t<detail::when_any::storage_type<detail::awaitable_result<Args>>>...>;

	/**
	 * @brief Alias for the result type of the Nth arg.
	 *
	 * @tparam N index of the argument to fetch
	 */
	template <size_t N>
	using result_t = std::variant_alternative_t<N + 1, variant_type>;

	/**
	 * @brief State shared between all the jobs to spawn
	 */
	struct state_t {
		/**
		 * @brief Constructor for the internal state. Its arguments are used to construct each awaitable
		 */
		template <typename... Args_>
		state_t(Args_&&... args) : awaitables{std::forward<Args_>(args)...} {}

		/**
		 * @brief Awaitable objects to handle.
		 */
		std::tuple<Args...> awaitables;

		/**
		 * @brief Result or exception, as a variant. This will contain the result of the first awaitable to finish
		 */
		variant_type result{};

		/**
		 * @brief Coroutine handle to resume after finishing an awaitable
		 */
		detail::std_coroutine::coroutine_handle<> handle{};

		/**
		 * @brief Index of the awaitable that finished. Initialized to the maximum value of std::size_t.
		 */
		size_t index_finished = std::numeric_limits<std::size_t>::max();

		/**
		 * @brief State of the when_any object.
		 *
		 * @see detail::when_any::await_state
		 */
		std::atomic<detail::when_any::await_state> owner_state{detail::when_any::await_state::started};
	};

	/**
	 * @brief Shared pointer to the state shared between the jobs spawned. Contains the awaitable objects and the result.
	 */
	std::shared_ptr<state_t> my_state{nullptr};

	/**
	 * @brief Spawn a dpp::job handling the Nth argument.
	 *
	 * @tparam N Index of the argument to handle
	 * @return dpp::job Job handling the Nth argument
	 */
	template <size_t N>
	static dpp::job make_job(std::shared_ptr<state_t> shared_state) {
		/**
		 * Any exceptions from the awaitable's await_suspend should be thrown to the caller (the coroutine creating the when_any object)
		 * If the co_await passes, and it is the first one to complete, try construct the result, catch any exceptions to rethrow at resumption, and resume.
		 */
		if constexpr (!std::same_as<result_t<N>, detail::when_any::empty>) {
			decltype(auto) result = co_await std::get<N>(shared_state->awaitables);

			if (auto s = shared_state->owner_state.load(std::memory_order_relaxed); s == detail::when_any::await_state::dangling || s == detail::when_any::await_state::done) {
				co_return;
			}

			using result_t = decltype(result);

			/* Try construct, prefer move if possible, store any exception to rethrow */
			try	{
				if constexpr (std::is_lvalue_reference_v<result_t> && !std::is_const_v<result_t> && std::is_move_constructible_v<std::remove_cvref_t<result_t>>) {
					shared_state->result.template emplace<N + 1>(std::move(result));
				} else {
					shared_state->result.template emplace<N + 1>(result);
				}
			} catch (...) {
				shared_state->result.template emplace<0>(std::current_exception());
			}
		} else {
			co_await std::get<N>(shared_state->awaitables);

			if (auto s = shared_state->owner_state.load(std::memory_order_relaxed); s == detail::when_any::await_state::dangling || s == detail::when_any::await_state::done) {
				co_return;
			}

			shared_state->result.template emplace<N + 1>();
		}

		if (shared_state->owner_state.exchange(detail::when_any::await_state::done) != detail::when_any::await_state::waiting) {
			co_return;
		}

		if (auto handle = shared_state->handle; handle) {
			shared_state->index_finished = N;
			shared_state->handle.resume();
		}
	}

	/**
	 * @brief Spawn a dpp::job to handle each awaitable.
	 * Each of them will co_await the awaitable and set the result if they are the first to finish
	 */
	void make_jobs() {
		[]<size_t... Ns>(when_any *self, std::index_sequence<Ns...>) {
			(make_job<Ns>(self->my_state), ...);
		}(this, std::index_sequence_for<Args...>{});
	}

public:
	/**
	 * @brief Object returned by \ref operator co_await() on resumption. Can be moved but not copied.
	 */
	class result {
		friend class when_any<Args...>;

		/**
		 * @brief Reference to the shared state to pull the data from
		 */
		std::shared_ptr<state_t> shared_state;

		/**
		 * @brief Default construction is deleted
		 */
		result() = delete;

		/**
		 * @brief Internal constructor taking the shared state
		 */
		result(std::shared_ptr<state_t> state) : shared_state{state} {}

	public:
		/**
		 * @brief Move constructor
		 */
		result(result&&) = default;

		/**
		 * @brief This object is not copyable.
		 */
		result(const result &) = delete;

		/**
		 * @brief Move assignment operator
		 */
		result &operator=(result&&) = default;

		/**
		 * @brief This object is not copyable.
		 */
		result &operator=(const result&) = delete;

		/**
		 * @brief Retrieve the index of the awaitable that finished first.
		 *
		 * @return size_t Index of the awaitable that finished first, relative to the template arguments of when_any
		 */
		size_t index() const noexcept {
			return shared_state->index_finished;
		}

		/**
		 * @brief Retrieve the non-void result of an awaitable.
		 *
		 * @tparam N Index of the result to retrieve. Must correspond to index().
		 * @throw ??? Throws any exception triggered at construction, or std::bad_variant_access if N does not correspond to index()
		 * @return Result of the awaitable as a reference.
		 */
		template <size_t N>
#ifndef _DOXYGEN_
		requires (!detail::when_any::void_result<result_t<N>>)
#endif
		result_t<N>& get() & {
			if (is_exception()) {
				std::rethrow_exception(std::get<0>(shared_state->result));
			}
			return std::get<N + 1>(shared_state->result);
		}

		/**
		 * @brief Retrieve the non-void result of an awaitable.
		 *
		 * @tparam N Index of the result to retrieve. Must correspond to index().
		 * @throw ??? Throws any exception triggered at construction, or std::bad_variant_access if N does not correspond to index()
		 * @return Result of the awaitable as a cpnst reference.
		 */
		template <size_t N>
#ifndef _DOXYGEN_
		requires (!detail::when_any::void_result<result_t<N>>)
#endif
		const result_t<N>& get() const& {
			if (is_exception()) {
				std::rethrow_exception(std::get<0>(shared_state->result));
			}
			return std::get<N + 1>(shared_state->result);
		}

		/**
		 * @brief Retrieve the non-void result of an awaitable.
		 *
		 * @tparam N Index of the result to retrieve. Must correspond to index().
		 * @throw ??? Throws any exception triggered at construction, or std::bad_variant_access if N does not correspond to index()
		 * @return Result of the awaitable as an rvalue reference.
		 */
		template <size_t N>
#ifndef _DOXYGEN_
		requires (!detail::when_any::void_result<result_t<N>>)
#endif
		result_t<N>&& get() && {
			if (is_exception()) {
				std::rethrow_exception(std::get<0>(shared_state->result));
			}
			return std::get<N + 1>(shared_state->result);
		}

		/**
		 * @brief Cannot retrieve a void result.
		 */
		template <size_t N>
#ifndef _DOXYGEN
		requires (detail::when_any::void_result<result_t<N>>)
#endif
		[[deprecated("cannot retrieve a void result")]] void get() = delete;

		/**
		 * @brief Checks whether the return of the first awaitable triggered an exception, that is, a call to get() will rethrow.
		 *
		 * @return Whether or not the result is an exception
		 */
		[[nodiscard]] bool is_exception() const noexcept {
			return shared_state->result.index() == 0;
		}
	};

	/**
	 * @brief Object returned by \ref operator co_await(). Meant to be used by the standard library, not by a user.
	 *
	 * @see result
	 */
	struct awaiter {
		/**
		 * @brief Pointer to the when_any object
		 */
		when_any *self;

		/**
		 * @brief First function called by the standard library when using co_await.
		 *
		 * @return bool Whether the result is ready
		 */
		[[nodiscard]] bool await_ready() const noexcept {
			return self->await_ready();
		}

		/**
		 * @brief Second function called by the standard library when using co_await.
		 *
		 * @return bool Returns false if we want to resume immediately.
		 */
		bool await_suspend(detail::std_coroutine::coroutine_handle<> caller) noexcept {
			auto sent = detail::when_any::await_state::started;
			self->my_state->handle = caller;
			return self->my_state->owner_state.compare_exchange_strong(sent, detail::when_any::await_state::waiting); // true (suspend) if `started` was replaced with `waiting` -- false (resume) if the value was not `started` (`done` is the only other option)
		}

		/**
		 * @brief Third and final function called by the standard library when using co_await. Returns the result object.
		 *
		 * @see result
		 */
		result await_resume() const noexcept {
			return {self->my_state};
		}
	};

	/**
	 * @brief Default constructor.
	 * A when_any object created this way holds no state
	 */
	when_any() = default;

	/**
	 * @brief Constructor from awaitable objects. Each awaitable is executed immediately and the when_any object can then be co_await-ed later.
	 *
	 * @throw ??? Any exception thrown by the start of each awaitable will propagate to the caller.
	 * @param args Arguments to construct each awaitable from. The when_any object will construct an awaitable for each, it is recommended to pass rvalues or std::move.
	 */
	template <typename... Args_>
#ifndef _DOXYGEN_
	requires (sizeof...(Args_) == sizeof...(Args))
#endif /* _DOXYGEN_ */
	when_any(Args_&&... args) : my_state{std::make_shared<state_t>(detail::when_any::arg_helper<Args_>::get(std::forward<Args_>(args))...)} {
		make_jobs();
	}

	/**
	 * @brief This object is not copyable.
	 */
	when_any(const when_any &) = delete;

	/**
	 * @brief Move constructor.
	 */
	when_any(when_any &&) noexcept = default;

	/**
	 * @brief On destruction the when_any will try to call @ref dpp::task::cancel() cancel() on each of its awaitable if they have such a method.
	 *
	 * @note If you are looking to use a custom type with when_any and want it to cancel on its destruction,
	 * make sure it has a cancel() method, which will trigger an await_resume() throwing a dpp::task_cancelled_exception.
	 * This object will swallow the exception and return cleanly. Any other exception will be thrown back to the resumer.
	 */
	~when_any() {
		if (!my_state)
			return;

		my_state->owner_state = detail::when_any::await_state::dangling;

		[]<size_t... Ns>(when_any *self, std::index_sequence<Ns...>) constexpr {
			constexpr auto cancel = []<size_t N>(when_any *self) constexpr {
				if constexpr (requires { std::get<N>(self->my_state->awaitables).cancel(); }) {
					try {
						std::get<N>(self->my_state->awaitables).cancel();
					} catch (...) {
						// swallow any exception. no choice here, we're in a destructor
					}
				}
			};
			(cancel.template operator()<Ns>(self), ...);
		}(this, std::index_sequence_for<Args...>());
	}

	/**
	 * @brief This object is not copyable.
	 */
	when_any &operator=(const when_any &) = delete;

	/**
	 * @brief Move assignment operator.
	 */
	when_any &operator=(when_any &&) noexcept = default;

	/**
	 * @brief Check whether a call to co_await would suspend.
	 *
	 * @note This can change from false to true at any point, but not the other way around.
	 * @return bool Whether co_await would suspend
	 */
	[[nodiscard]] bool await_ready() const noexcept {
		return my_state->owner_state == detail::when_any::await_state::done;
	}

	/**
	 * @brief Suspend the caller until any of the awaitables completes.
	 *
	 * @see result
	 * @throw ??? On resumption, throws any exception caused by the construction of the result.
	 * @return result On resumption, this object returns an object that allows to retrieve the index and result of the awaitable.
	 */
	[[nodiscard]] awaiter operator co_await() noexcept {
		return {this};
	}
};

template <typename... Args>
#ifndef _DOXYGEN_
requires (sizeof...(Args) >= 1)
#endif /* _DOXYGEN_ */
when_any(Args...) -> when_any<detail::when_any::awaitable_type<Args>...>;

} /* namespace dpp */

#endif