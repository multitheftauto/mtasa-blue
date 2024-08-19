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

struct async_dummy {
	int* dummy_shared_state = nullptr;
};

}

#ifdef DPP_CORO

#include "coro.h"

#include <mutex>
#include <utility>
#include <type_traits>
#include <functional>
#include <atomic>
#include <cstddef>

namespace dpp {

namespace detail {

/**
 * @brief Empty struct used for overload resolution.
 */
struct empty_tag_t{};

namespace async {

/**
 * @brief Represents the step an std::async is at.
 */
enum class state_t {
	/**
	 * @brief Request was sent but not co_await-ed. handle is nullptr, result_storage is not constructed.
	 */
	sent,

	/**
	 * @brief Request was co_await-ed. handle is valid, result_storage is not constructed.
	 */
	waiting,

	/**
	 * @brief Request was completed. handle is unknown, result_storage is valid.
	 */
	done,

	/**
	 * @brief Request was never co_await-ed.
	 */
	dangling
};

/**
 * @brief State of the async and its callback.
 *
 * Defined outside of dpp::async because this seems to work better with Intellisense.
 */
template <typename R>
struct async_callback_data {
	/**
	 * @brief Number of references to this callback state.
	 */
	std::atomic<int> ref_count{1};

	/**
	 * @brief State of the awaitable and the API callback
	 */
	std::atomic<state_t> state = state_t::sent;

	/**
	 * @brief The stored result of the API call, stored as an array of bytes to directly construct in place
	 */
	alignas(R) std::array<std::byte, sizeof(R)> result_storage;

	/**
	 * @brief Handle to the coroutine co_await-ing on this API call
	 *
	 * @see <a href="https://en.cppreference.com/w/cpp/coroutine/coroutine_handle">std::coroutine_handle</a>
	 */
	std_coroutine::coroutine_handle<> coro_handle = nullptr;

	/**
	 * @brief Convenience function to construct the result in the storage and initialize its lifetime
	 *
	 * @warning This is only a convenience function, ONLY CALL THIS IN THE CALLBACK, before setting state to done.
	 */
	template <typename... Ts>
	void construct_result(Ts&&... ts) {
		// Standard-compliant type punning yay
		std::construct_at<R>(reinterpret_cast<R *>(result_storage.data()), std::forward<Ts>(ts)...);
	}

	/**
	 * @brief Destructor.
	 *
	 * Also destroys the result if present.
	 */
	~async_callback_data() {
		if (state.load() == state_t::done) {
			std::destroy_at<R>(reinterpret_cast<R *>(result_storage.data()));
		}
	}
};

/**
 * @brief Base class of dpp::async.
 *
 * @warning This class should not be used directly by a user, use dpp::async instead.
 * @note This class contains all the functions used internally by co_await. It is intentionally opaque and a private base of dpp::async so a user cannot call await_suspend and await_resume directly.
 */
template <typename R>
class async_base {
	/**
	 * @brief Ref-counted callback, contains the callback logic and manages the lifetime of the callback data over multiple threads.
	 */
	struct shared_callback {
		/**
		 * @brief Self-managed ref-counted pointer to the state data
		 */
		async_callback_data<R> *state = new async_callback_data<R>;

		/**
		 * @brief Callback function.
		 *
		 * Constructs the callback data, and if the coroutine was awaiting, resume it
		 * @param cback The result of the API call.
		 * @tparam V Forwarding reference convertible to R
		 */
		template <std::convertible_to<R> V>
		void operator()(V &&cback) const {
			state->construct_result(std::forward<V>(cback));
			if (auto previous_state = state->state.exchange(state_t::done); previous_state == state_t::waiting) {
				state->coro_handle.resume();
			}
		}

		/**
		 * @brief Main constructor, allocates a new callback_state object.
		 */
		shared_callback() = default;

		/**
		 * @brief Empty constructor, holds no state.
		 */
		explicit shared_callback(detail::empty_tag_t) noexcept : state{nullptr} {}

		/**
		 * @brief Copy constructor. Takes shared ownership of the callback state, increasing the reference count.
		 */
		shared_callback(const shared_callback &other) noexcept {
			this->operator=(other);
		}

		/**
		 * @brief Move constructor. Transfers ownership from another object, leaving intact the reference count. The other object releases the callback state.
		 */
		shared_callback(shared_callback &&other) noexcept {
			this->operator=(std::move(other));
		}

		/**
		 * @brief Destructor. Releases the held reference and destroys if no other references exist.
		 */
		~shared_callback() {
			if (!state) { // Moved-from object
				return;
			}

			auto count = state->ref_count.fetch_sub(1);
			if (count == 0) {
				delete state;
			}
		}

		/**
		 * @brief Copy assignment. Takes shared ownership of the callback state, increasing the reference count.
		 */
		shared_callback &operator=(const shared_callback &other) noexcept {
			state = other.state;
			++state->ref_count;
			return *this;
		}

		/**
		 * @brief Move assignment. Transfers ownership from another object, leaving intact the reference count. The other object releases the callback state.
		 */
		shared_callback &operator=(shared_callback &&other) noexcept {
			state = std::exchange(other.state, nullptr);
			return *this;
		}

		/**
		 * @brief Function called by the async when it is destroyed when it was never co_awaited, signals to the callback to abort.
		 */
		void set_dangling() noexcept {
			if (!state) { // moved-from object
				return;
			}
			state->state.store(state_t::dangling);
		}

		bool done(std::memory_order order = std::memory_order_seq_cst) const noexcept {
			return (state->state.load(order) == state_t::done);
		}

		/**
		 * @brief Convenience function to get the shared callback state's result.
		 *
		 * @warning It is UB to call this on a callback whose state is anything else but state_t::done.
		 */
		R &get_result() noexcept {
			assert(state && done());
			return (*reinterpret_cast<R *>(state->result_storage.data()));
		}

		/**
		 * @brief Convenience function to get the shared callback state's result.
		 *
		 * @warning It is UB to call this on a callback whose state is anything else but state_t::done.
		 */
		const R &get_result() const noexcept {
			assert(state && done());
			return (*reinterpret_cast<R *>(state->result_storage.data()));
		}
	};

	/**
	 * @brief Shared state of the async and its callback, to be used across threads.
	 */
	shared_callback api_callback{nullptr};

public:
	/**
	 * @brief Construct an async object wrapping an object method, the call is made immediately by forwarding to <a href="https://en.cppreference.com/w/cpp/utility/functional/invoke">std::invoke</a> and can be awaited later to retrieve the result.
	 *
	 * @param obj The object to call the method on
	 * @param fun The method of the object to call. Its last parameter must be a callback taking a parameter of type R
	 * @param args Parameters to pass to the method, excluding the callback
	 */
	template <typename Obj, typename Fun, typename... Args>
#ifndef _DOXYGEN_
	requires std::invocable<Fun, Obj, Args..., std::function<void(R)>>
#endif
	explicit async_base(Obj &&obj, Fun &&fun, Args&&... args) : api_callback{} {
		std::invoke(std::forward<Fun>(fun), std::forward<Obj>(obj), std::forward<Args>(args)..., api_callback);
	}

	/**
	 * @brief Construct an async object wrapping an invokeable object, the call is made immediately by forwarding to <a href="https://en.cppreference.com/w/cpp/utility/functional/invoke">std::invoke</a> and can be awaited later to retrieve the result.
	 *
	 * @param fun The object to call using <a href="https://en.cppreference.com/w/cpp/utility/functional/invoke">std::invoke</a>. Its last parameter must be a callable taking a parameter of type R
	 * @param args Parameters to pass to the object, excluding the callback
	 */
	template <typename Fun, typename... Args>
#ifndef _DOXYGEN_
	requires std::invocable<Fun, Args..., std::function<void(R)>>
#endif
	explicit async_base(Fun &&fun, Args&&... args) : api_callback{} {
		std::invoke(std::forward<Fun>(fun), std::forward<Args>(args)..., api_callback);
	}

	/**
	 * @brief Construct an empty async. Using `co_await` on an empty async is undefined behavior.
	 */
	async_base() noexcept : api_callback{detail::empty_tag_t{}} {}

	/**
	 * @brief Destructor. If any callback is pending it will be aborted.
	 */
	~async_base() {
		api_callback.set_dangling();
	}

	/**
	 * @brief Copy constructor is disabled
	 */
	async_base(const async_base &) = delete;

	/**
	 * @brief Move constructor
	 *
	 * NOTE: Despite being marked noexcept, this function uses std::lock_guard which may throw. The implementation assumes this can never happen, hence noexcept. Report it if it does, as that would be a bug.
	 *
	 * @remark Using the moved-from async after this function is undefined behavior.
	 * @param other The async object to move the data from.
	 */
	async_base(async_base &&other) noexcept = default;

	/**
	 * @brief Copy assignment is disabled
	 */
	async_base &operator=(const async_base &) = delete;

	/**
	 * @brief Move assignment operator.
	 *
	 * NOTE: Despite being marked noexcept, this function uses std::lock_guard which may throw. The implementation assumes this can never happen, hence noexcept. Report it if it does, as that would be a bug.
	 *
	 * @remark Using the moved-from async after this function is undefined behavior.
	 * @param other The async object to move the data from
	 */
	async_base &operator=(async_base &&other) noexcept = default;

	/**
	 * @brief Check whether or not co_await-ing this would suspend the caller, i.e. if we have the result or not
	 *
	 * @return bool Whether we already have the result of the API call or not
	 */
	[[nodiscard]] bool await_ready() const noexcept {
		return api_callback.done();
	}

	/**
	 * @brief Second function called by the standard library when the object is co-awaited, if await_ready returned false.
	 *
	 * Checks again for the presence of the result, if absent, signals to suspend and keep track of the calling coroutine for the callback to resume.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @param caller The handle to the coroutine co_await-ing and being suspended
	 */
	[[nodiscard]] bool await_suspend(detail::std_coroutine::coroutine_handle<> caller) noexcept {
		auto sent = state_t::sent;
		api_callback.state->coro_handle = caller;
		return api_callback.state->state.compare_exchange_strong(sent, state_t::waiting); // true (suspend) if `sent` was replaced with `waiting` -- false (resume) if the value was not `sent` (`done` is the only other option)
	}

	/**
	 * @brief Function called by the standard library when the async is resumed. Its return value is what the whole co_await expression evaluates to
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @return The result of the API call as an lvalue reference.
	 */
	R& await_resume() & noexcept {
		return api_callback.get_result();
	}


	/**
	 * @brief Function called by the standard library when the async is resumed. Its return value is what the whole co_await expression evaluates to
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @return The result of the API call as a const lvalue reference.
	 */
	const R& await_resume() const& noexcept {
		return api_callback.get_result();
	}

	/**
	 * @brief Function called by the standard library when the async is resumed. Its return value is what the whole co_await expression evaluates to
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @return The result of the API call as an rvalue reference.
	 */
	R&& await_resume() && noexcept {
		return std::move(api_callback.get_result());
	}
};

} // namespace async

} // namespace detail

struct confirmation_callback_t;

/**
 * @class async async.h coro/async.h
 * @brief A co_await-able object handling an API call in parallel with the caller.
 *
 * This class is the return type of the dpp::cluster::co_* methods, but it can also be created manually to wrap any async call.
 *
 * @remark - The coroutine may be resumed in another thread, do not rely on thread_local variables.
 * @warning - This feature is EXPERIMENTAL. The API may change at any time and there may be bugs. Please report any to <a href="https://github.com/brainboxdotcc/DPP/issues">GitHub issues</a> or to the <a href="https://discord.gg/dpp">D++ Discord server</a>.
 * @tparam R The return type of the API call. Defaults to confirmation_callback_t
 */
template <typename R>
class async : private detail::async::async_base<R> {
	/**
	 * @brief Internal use only base class. It serves to prevent await_suspend and await_resume from being used directly.
	 *
	 * @warning For internal use only, do not use.
	 * @see operator co_await()
	 */
	friend class detail::async::async_base<R>;

public:
	using detail::async::async_base<R>::async_base; // use async_base's constructors. unfortunately on clang this doesn't include the templated ones so we have to delegate below
	using detail::async::async_base<R>::operator=; // use async_base's assignment operator
	using detail::async::async_base<R>::await_ready; // expose await_ready as public

	/**
	 * @brief Construct an async object wrapping an object method, the call is made immediately by forwarding to <a href="https://en.cppreference.com/w/cpp/utility/functional/invoke">std::invoke</a> and can be awaited later to retrieve the result.
	 *
	 * @param obj The object to call the method on
	 * @param fun The method of the object to call. Its last parameter must be a callback taking a parameter of type R
	 * @param args Parameters to pass to the method, excluding the callback
	 */
	template <typename Obj, typename Fun, typename... Args>
#ifndef _DOXYGEN_
	requires std::invocable<Fun, Obj, Args..., std::function<void(R)>>
#endif
	explicit async(Obj &&obj, Fun &&fun, Args&&... args) : detail::async::async_base<R>{std::forward<Obj>(obj), std::forward<Fun>(fun), std::forward<Args>(args)...} {}

	/**
	 * @brief Construct an async object wrapping an invokeable object, the call is made immediately by forwarding to <a href="https://en.cppreference.com/w/cpp/utility/functional/invoke">std::invoke</a> and can be awaited later to retrieve the result.
	 *
	 * @param fun The object to call using <a href="https://en.cppreference.com/w/cpp/utility/functional/invoke">std::invoke</a>. Its last parameter must be a callable taking a parameter of type R
	 * @param args Parameters to pass to the object, excluding the callback
	 */
	template <typename Fun, typename... Args>
#ifndef _DOXYGEN_
	requires std::invocable<Fun, Args..., std::function<void(R)>>
#endif
	explicit async(Fun &&fun, Args&&... args) : detail::async::async_base<R>{std::forward<Fun>(fun), std::forward<Args>(args)...} {}

#ifdef _DOXYGEN_ // :)
	/**
	 * @brief Construct an empty async. Using `co_await` on an empty async is undefined behavior.
	 */
	async() noexcept;

	/**
	 * @brief Destructor. If any callback is pending it will be aborted.
	 */
	~async();

	/**
	 * @brief Copy constructor is disabled
	 */
	async(const async &);

	/**
	 * @brief Move constructor
	 *
	 * NOTE: Despite being marked noexcept, this function uses std::lock_guard which may throw. The implementation assumes this can never happen, hence noexcept. Report it if it does, as that would be a bug.
	 *
	 * @remark Using the moved-from async after this function is undefined behavior.
	 * @param other The async object to move the data from.
	 */
	async(async &&other) noexcept = default;

	/**
	 * @brief Copy assignment is disabled
	 */
	async &operator=(const async &) = delete;

	/**
	 * @brief Move assignment operator.
	 *
	 * NOTE: Despite being marked noexcept, this function uses std::lock_guard which may throw. The implementation assumes this can never happen, hence noexcept. Report it if it does, as that would be a bug.
	 *
	 * @remark Using the moved-from async after this function is undefined behavior.
	 * @param other The async object to move the data from
	 */
	async &operator=(async &&other) noexcept = default;

	/**
	 * @brief Check whether or not co_await-ing this would suspend the caller, i.e. if we have the result or not
	 *
	 * @return bool Whether we already have the result of the API call or not
	 */
	[[nodiscard]] bool await_ready() const noexcept;
#endif

	/**
	 * @brief Suspend the caller until the request completes.
	 *
	 * @return On resumption, this expression evaluates to the result object of type R, as a reference.
	 */
	[[nodiscard]] auto& operator co_await() & noexcept {
		return static_cast<detail::async::async_base<R>&>(*this);
	}

	/**
	 * @brief Suspend the caller until the request completes.
	 *
	 * @return On resumption, this expression evaluates to the result object of type R, as a const reference.
	 */
	[[nodiscard]] const auto& operator co_await() const & noexcept {
		return static_cast<detail::async::async_base<R> const&>(*this);
	}

	/**
	 * @brief Suspend the caller until the request completes.
	 *
	 * @return On resumption, this expression evaluates to the result object of type R, as an rvalue reference.
	 */
	[[nodiscard]] auto&& operator co_await() && noexcept {
		return static_cast<detail::async::async_base<R>&&>(*this);
	}
};

DPP_CHECK_ABI_COMPAT(async<>, async_dummy);

} // namespace dpp

#endif /* DPP_CORO */
