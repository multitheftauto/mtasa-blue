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

struct coroutine_dummy {
	int *handle_dummy = nullptr;
};

}

#ifdef DPP_CORO

#include "coro.h"

#include <optional>
#include <type_traits>
#include <exception>
#include <utility>
#include <type_traits>

namespace dpp {

namespace detail {

namespace coroutine {

	template <typename R>
struct promise_t;

template <typename R>
/**
 * @brief Alias for the handle_t of a coroutine.
 */
using handle_t = std_coroutine::coroutine_handle<promise_t<R>>;

/**
 * @brief Base class of dpp::coroutine<R>.
 *
 * @warning This class should not be used directly by a user, use dpp::coroutine<R> instead.
 * @note This class contains all the functions used internally by co_await. It is intentionally opaque and a private base of dpp::coroutine<R> so a user cannot call await_suspend and await_resume directly.
 */
template <typename R>
class coroutine_base {
protected:
	/**
	 * @brief Promise has friend access for the constructor
	 */
	friend struct promise_t<R>;

	/**
	 * @brief Coroutine handle.
	 */
	detail::coroutine::handle_t<R> handle{nullptr};

private:
	/**
	 * @brief Construct from a handle. Internal use only.
	 */
	coroutine_base(detail::coroutine::handle_t<R> h) : handle{h} {}

public:
	/**
	 * @brief Default constructor, creates an empty coroutine.
	 */
	coroutine_base() = default;

	/**
	 * @brief Copy constructor is disabled
	 */
	coroutine_base(const coroutine_base &) = delete;

	/**
	 * @brief Move constructor, grabs another coroutine's handle
	 *
	 * @param other Coroutine to move the handle from
	 */
	coroutine_base(coroutine_base &&other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

	/**
	 * @brief Destructor, destroys the handle.
	 */
	~coroutine_base() {
		if (handle) {
			handle.destroy();
		}
	}

	/**
	 * @brief Copy assignment is disabled
	 */
	coroutine_base &operator=(const coroutine_base &) = delete;

	/**
	 * @brief Move assignment, grabs another coroutine's handle
	 *
	 * @param other Coroutine to move the handle from
	 */
	coroutine_base &operator=(coroutine_base &&other) noexcept {
		handle = std::exchange(other.handle, nullptr);
		return *this;
	}

	/**
	 * @brief First function called by the standard library when the coroutine is co_await-ed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @throws invalid_operation_exception if the coroutine is empty or finished.
	 * @return bool Whether the coroutine is done
	 */
	[[nodiscard]] bool await_ready() const {
		if (!handle) {
			throw dpp::logic_exception("cannot co_await an empty coroutine");
		}
		return handle.done();
	}

	/**
	 * @brief Second function called by the standard library when the coroutine is co_await-ed.
	 *
	 * Stores the calling coroutine in the promise to resume when this coroutine suspends.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @param caller The calling coroutine, now suspended
	 */
	template <typename T>
	[[nodiscard]] handle_t<R> await_suspend(detail::std_coroutine::coroutine_handle<T> caller) noexcept {
		handle.promise().parent = caller;
		return handle;
	}

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 * @return R The result of the coroutine. It is given to the caller as a result to `co_await`
	 */
	decltype(auto) await_resume() & {
		return static_cast<dpp::coroutine<R> &>(*this).await_resume_impl();
	}

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 * @return R The result of the coroutine. It is given to the caller as a result to `co_await`
	 */
	[[nodiscard]] decltype(auto) await_resume() const & {
		return static_cast<dpp::coroutine<R> const&>(*this).await_resume_impl();
	}

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 * @return R The result of the coroutine. It is given to the caller as a result to `co_await`
	 */
	[[nodiscard]] decltype(auto) await_resume() && {
		return static_cast<dpp::coroutine<R> &&>(*this).await_resume_impl();
	}
};

} // namespace coroutine

} // namespace detail

/**
 * @class coroutine coroutine.h coro/coroutine.h
 * @brief Base type for a coroutine, starts on co_await.
 *
 * @warning - This feature is EXPERIMENTAL. The API may change at any time and there may be bugs.
 * Please report any to <a href="https://github.com/brainboxdotcc/DPP/issues">GitHub Issues</a> or to our <a href="https://discord.gg/dpp">Discord Server</a>.
 * @warning - Using co_await on this object more than once is undefined behavior.
 * @tparam R Return type of the coroutine. Can be void, or a complete object that supports move construction and move assignment.
 */
template <typename R>
class coroutine : private detail::coroutine::coroutine_base<R> {
	/**
	 * @brief Internal use only base class containing common logic between coroutine<R> and coroutine<void>. It also serves to prevent await_suspend and await_resume from being used directly.
	 *
	 * @warning For internal use only, do not use.
	 * @see operator co_await()
	 */
	friend class detail::coroutine::coroutine_base<R>;

	[[nodiscard]] R& await_resume_impl() & {
		detail::coroutine::promise_t<R> &promise = this->handle.promise();
		if (promise.exception) {
			std::rethrow_exception(promise.exception);
		}
		return *promise.result;
	}

	[[nodiscard]] const R& await_resume_impl() const & {
		detail::coroutine::promise_t<R> &promise = this->handle.promise();
		if (promise.exception) {
			std::rethrow_exception(promise.exception);
		}
		return *promise.result;
	}

	[[nodiscard]] R&& await_resume_impl() && {
		detail::coroutine::promise_t<R> &promise = this->handle.promise();
		if (promise.exception) {
			std::rethrow_exception(promise.exception);
		}
		return *std::move(promise.result);
	}

public:
#ifdef _DOXYGEN_ // :))))
	/**
	 * @brief Default constructor, creates an empty coroutine.
	 */
	coroutine() = default;

	/**
	 * @brief Copy constructor is disabled
	 */
	coroutine(const coroutine &) = delete;

	/**
	 * @brief Move constructor, grabs another coroutine's handle
	 *
	 * @param other Coroutine to move the handle from
	 */
	coroutine(coroutine &&other) noexcept;

	/**
	 * @brief Destructor, destroys the handle.
	 */
	~coroutine();

	/**
	 * @brief Copy assignment is disabled
	 */
	coroutine &operator=(const coroutine &) = delete;

	/**
	 * @brief Move assignment, grabs another coroutine's handle
	 *
	 * @param other Coroutine to move the handle from
	 */
	coroutine &operator=(coroutine &&other) noexcept;

	/**
	 * @brief First function called by the standard library when the coroutine is co_await-ed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @throws invalid_operation_exception if the coroutine is empty or finished.
	 * @return bool Whether the coroutine is done
	 */
	[[nodiscard]] bool await_ready() const;
#else
	using detail::coroutine::coroutine_base<R>::coroutine_base; // use coroutine_base's constructors
	using detail::coroutine::coroutine_base<R>::operator=; // use coroutine_base's assignment operators
	using detail::coroutine::coroutine_base<R>::await_ready; // expose await_ready as public
#endif

	/**
	 * @brief Suspend the caller until the coroutine completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as a reference.
	 */
	[[nodiscard]] auto& operator co_await() & noexcept {
		return static_cast<detail::coroutine::coroutine_base<R>&>(*this);
	}

	/**
	 * @brief Suspend the caller until the coroutine completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as a const reference.
	 */
	[[nodiscard]] const auto& operator co_await() const & noexcept {
		return static_cast<detail::coroutine::coroutine_base<R> const&>(*this);
	}

	/**
	 * @brief Suspend the caller until the coroutine completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as an rvalue reference.
	 */
	[[nodiscard]] auto&& operator co_await() && noexcept {
		return static_cast<detail::coroutine::coroutine_base<R>&&>(*this);
	}
};

#ifndef _DOXYGEN_ // don't generate this on doxygen because `using` doesn't work and 2 copies of coroutine_base's docs is enough
/**
 * @brief Base type for a coroutine, starts on co_await.
 *
 * @warning - This feature is EXPERIMENTAL. The API may change at any time and there may be bugs. Please report any to <a href="https://github.com/brainboxdotcc/DPP/issues">GitHub issues</a> or to the <a href="https://discord.gg/dpp">D++ Discord server</a>.
 * @warning - Using co_await on this object more than once is undefined behavior.
 * @tparam R Return type of the coroutine. Can be void, or a complete object that supports move construction and move assignment.
 */
template <>
class coroutine<void> : private detail::coroutine::coroutine_base<void> {
	/**
	 * @brief Base class has friend access for CRTP downcast
	 */
	friend class detail::coroutine::coroutine_base<void>;

	void await_resume_impl() const;

public:
	using detail::coroutine::coroutine_base<void>::coroutine_base; // use coroutine_base's constructors
	using detail::coroutine::coroutine_base<void>::operator=; // use coroutine_base's assignment operators
	using detail::coroutine::coroutine_base<void>::await_ready; // expose await_ready as public

	/**
	 * @brief Suspend the current coroutine until the coroutine completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as a reference.
	 */
	[[nodiscard]] auto& operator co_await() & noexcept {
		return static_cast<detail::coroutine::coroutine_base<void>&>(*this);
	}

	/**
	 * @brief Suspend the current coroutine until the coroutine completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as a const reference.
	 */
	[[nodiscard]] const auto& operator co_await() const & noexcept {
		return static_cast<detail::coroutine::coroutine_base<void> const &>(*this);
	}

	/**
	 * @brief Suspend the current coroutine until the coroutine completes.
	 *
	 * @throw On resumption, any exception thrown by the coroutine is propagated to the caller.
	 * @return On resumption, this expression evaluates to the result object of type R, as an rvalue reference.
	 */
	[[nodiscard]] auto&& operator co_await() && noexcept {
		return static_cast<detail::coroutine::coroutine_base<void>&&>(*this);
	}
};
#endif /* _DOXYGEN_ */

namespace detail::coroutine {
	template <typename R>
	struct final_awaiter;

#ifdef DPP_CORO_TEST
	struct promise_t_base{};
#endif

	/**
	 * @brief Promise type for coroutine.
	 */
	template <typename R>
	struct promise_t {
		/**
		 * @brief Handle of the coroutine co_await-ing this coroutine.
		 */
		std_coroutine::coroutine_handle<> parent{nullptr};

		/**
		 * @brief Return value of the coroutine
		 */
		std::optional<R> result{};

		/**
		 * @brief Pointer to an uncaught exception thrown by the coroutine
		 */
		std::exception_ptr exception{nullptr};

#ifdef DPP_CORO_TEST
		promise_t() {
			++coro_alloc_count<promise_t_base>;
		}

		~promise_t() {
			--coro_alloc_count<promise_t_base>;
		}
#endif

		/**
		 * @brief Function called by the standard library when reaching the end of a coroutine
		 *
		 * @return final_awaiter<R> Resumes any coroutine co_await-ing on this
		 */
		[[nodiscard]] final_awaiter<R> final_suspend() const noexcept;

		/**
		 * @brief Function called by the standard library when the coroutine start
		 *
		 * @return @return <a href="https://en.cppreference.com/w/cpp/coroutine/suspend_always">std::suspend_always</a> Always suspend at the start, for a lazy start
		 */
		[[nodiscard]] std_coroutine::suspend_always initial_suspend() const noexcept {
			return {};
		}

		/**
		 * @brief Function called when an exception escapes the coroutine
		 *
		 * Stores the exception to throw to the co_await-er
		 */
		void unhandled_exception() noexcept {
			exception = std::current_exception();
		}

		/**
		 * @brief Function called by the standard library when the coroutine co_returns a value.
		 *
		 * Stores the value internally to hand to the caller when it resumes.
		 *
		 * @param expr The value given to co_return
		 */
		void return_value(R&& expr) noexcept(std::is_nothrow_move_constructible_v<R>) requires std::move_constructible<R> {
			result = static_cast<R&&>(expr);
		}

		/**
		 * @brief Function called by the standard library when the coroutine co_returns a value.
		 *
		 * Stores the value internally to hand to the caller when it resumes.
		 *
		 * @param expr The value given to co_return
		 */
		void return_value(const R &expr) noexcept(std::is_nothrow_copy_constructible_v<R>) requires std::copy_constructible<R> {
			result = expr;
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
			result = std::forward<T>(expr);
		}

		/**
		 * @brief Function called to get the coroutine object
		 */
		dpp::coroutine<R> get_return_object() {
			return dpp::coroutine<R>{handle_t<R>::from_promise(*this)};
		}
	};

	/**
	 * @brief Struct returned by a coroutine's final_suspend, resumes the continuation
	 */
	template <typename R>
	struct final_awaiter {
		/**
		 * @brief First function called by the standard library when reaching the end of a coroutine
		 *
		 * @return false Always return false, we need to suspend to resume the parent
		 */
		[[nodiscard]] bool await_ready() const noexcept {
			return false;
		}

		/**
		 * @brief Second function called by the standard library when reaching the end of a coroutine.
		 *
		 * @return std::handle_t<> Coroutine handle to resume, this is either the parent if present or std::noop_coroutine()
		 */
		[[nodiscard]] std_coroutine::coroutine_handle<> await_suspend(std_coroutine::coroutine_handle<promise_t<R>> handle) const noexcept {
			auto parent = handle.promise().parent;

			return parent ? parent : std_coroutine::noop_coroutine();
		}

		/**
		 * @brief Function called by the standard library when this object is resumed
		 */
		void await_resume() const noexcept {}
	};

	template <typename R>
	final_awaiter<R> promise_t<R>::final_suspend() const noexcept {
		return {};
	}

	/**
	 * @brief Struct returned by a coroutine's final_suspend, resumes the continuation
	 */
	template <>
	struct promise_t<void> {
		/**
		 * @brief Handle of the coroutine co_await-ing this coroutine.
		 */
		std_coroutine::coroutine_handle<> parent{nullptr};

		/**
		 * @brief Pointer to an uncaught exception thrown by the coroutine
		 */
		std::exception_ptr exception{nullptr};

		/**
		 * @brief Function called by the standard library when reaching the end of a coroutine
		 *
		 * @return final_awaiter<R> Resumes any coroutine co_await-ing on this
		 */
		[[nodiscard]] final_awaiter<void> final_suspend() const noexcept {
			return {};
		}

		/**
		 * @brief Function called by the standard library when the coroutine start
		 *
		 * @return @return <a href="https://en.cppreference.com/w/cpp/coroutine/suspend_always">std::suspend_always</a> Always suspend at the start, for a lazy start
		 */
		[[nodiscard]] std_coroutine::suspend_always initial_suspend() const noexcept {
			return {};
		}

		/**
		 * @brief Function called when an exception escapes the coroutine
		 *
		 * Stores the exception to throw to the co_await-er
		 */
		void unhandled_exception() noexcept {
			exception = std::current_exception();
		}

		/**
		 * @brief Function called when co_return is used
		 */
		void return_void() const noexcept {}

		/**
		 * @brief Function called to get the coroutine object
		 */
		[[nodiscard]] dpp::coroutine<void> get_return_object() {
			return dpp::coroutine<void>{handle_t<void>::from_promise(*this)};
		}
	};

} // namespace detail

#ifndef _DOXYGEN_
inline void coroutine<void>::await_resume_impl() const {
	if (handle.promise().exception) {
		std::rethrow_exception(handle.promise().exception);
	}
}
#endif /* _DOXYGEN_ */

DPP_CHECK_ABI_COMPAT(coroutine<void>, coroutine_dummy)
DPP_CHECK_ABI_COMPAT(coroutine<uint64_t>, coroutine_dummy)

} // namespace dpp

/**
 * @brief Specialization of std::coroutine_traits, helps the standard library figure out a promise type from a coroutine function.
 */
template<typename R, typename... Args>
struct dpp::detail::std_coroutine::coroutine_traits<dpp::coroutine<R>, Args...> {
	using promise_type = dpp::detail::coroutine::promise_t<R>;
};

#endif /* DPP_CORO */
