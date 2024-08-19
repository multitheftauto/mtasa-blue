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

#if (defined(_LIBCPP_VERSION) and !defined(__cpp_impl_coroutine)) // if libc++ experimental implementation (LLVM < 14)
#  define STDCORO_EXPERIMENTAL_HEADER
#  define STDCORO_EXPERIMENTAL_NAMESPACE
#endif

#ifdef STDCORO_GLIBCXX_COMPAT
#  define __cpp_impl_coroutine 1
namespace std {
	namespace experimental {
		using namespace std;
	}
}
#endif

#ifdef STDCORO_EXPERIMENTAL_HEADER
#  include <experimental/coroutine>
#else
#  include <coroutine>
#endif

namespace dpp {

/**
 * @brief Implementation details for internal use only.
 *
 * @attention This is only meant to be used by D++ internally. Support will not be given regarding the facilities in this namespace.
 */
namespace detail {
#ifdef _DOXYGEN_
/**
 * @brief Alias for either std or std::experimental depending on compiler and library. Used by coroutine implementation.
 *
 * @todo Remove and use std when all supported libraries have coroutines in it
 */
namespace std_coroutine {}
#else
#  ifdef STDCORO_EXPERIMENTAL_NAMESPACE
namespace std_coroutine = std::experimental;
#  else
namespace std_coroutine = std;
#  endif
#endif

#ifndef _DOXYGEN_
/**
 * @brief Concept to check if a type has a useable `operator co_await()` member
 */
template <typename T>
concept has_co_await_member = requires (T expr) { expr.operator co_await(); };

/**
 * @brief Concept to check if a type has a useable overload of the free function `operator co_await(expr)`
 */
template <typename T>
concept has_free_co_await = requires (T expr) { operator co_await(expr); };

/**
 * @brief Concept to check if a type has useable `await_ready()`, `await_suspend()` and `await_resume()` member functions.
 */
template <typename T>
concept has_await_members = requires (T expr) { expr.await_ready(); expr.await_suspend(); expr.await_resume(); };

/**
 * @brief Mimics the compiler's behavior of using co_await. That is, it returns whichever works first, in order : `expr.operator co_await();` > `operator co_await(expr)` > `expr`
 */
template <typename T>
requires (has_co_await_member<T>)
decltype(auto) co_await_resolve(T&& expr) noexcept(noexcept(expr.operator co_await())) {
	decltype(auto) awaiter = expr.operator co_await();
	return awaiter;
}

/**
 * @brief Mimics the compiler's behavior of using co_await. That is, it returns whichever works first, in order : `expr.operator co_await();` > `operator co_await(expr)` > `expr`
 */
template <typename T>
requires (!has_co_await_member<T> && has_free_co_await<T>)
decltype(auto) co_await_resolve(T&& expr) noexcept(noexcept(operator co_await(expr))) {
	decltype(auto) awaiter = operator co_await(static_cast<T&&>(expr));
	return awaiter;
}

/**
 * @brief Mimics the compiler's behavior of using co_await. That is, it returns whichever works first, in order : `expr.operator co_await();` > `operator co_await(expr)` > `expr`
 */
template <typename T>
requires (!has_co_await_member<T> && !has_free_co_await<T>)
decltype(auto) co_await_resolve(T&& expr) noexcept {
	return static_cast<T&&>(expr);
}

#else
/**
 * @brief Concept to check if a type has a useable `operator co_await()` member
 *
 * @note This is actually a C++20 concept but Doxygen doesn't do well with them
 */
template <typename T>
bool has_co_await_member;

/**
 * @brief Concept to check if a type has a useable overload of the free function `operator co_await(expr)`
 *
 * @note This is actually a C++20 concept but Doxygen doesn't do well with them
 */
template <typename T>
bool has_free_co_await;

/**
 * @brief Concept to check if a type has useable `await_ready()`, `await_suspend()` and `await_resume()` member functions.
 *
 * @note This is actually a C++20 concept but Doxygen doesn't do well with them
 */
template <typename T>
bool has_await_members;

/**
 * @brief Mimics the compiler's behavior of using co_await. That is, it returns whichever works first, in order : `expr.operator co_await();` > `operator co_await(expr)` > `expr`
 *
 * This function is conditionally noexcept, if the returned expression also is.
 */
decltype(auto) co_await_resolve(auto&& expr) {}
#endif

/**
 * @brief Convenience alias for the result of a certain awaitable's await_resume.
 */
template <typename T>
using awaitable_result = decltype(co_await_resolve(std::declval<T>()).await_resume());

} // namespace detail

struct confirmation_callback_t;

template <typename R = confirmation_callback_t>
class async;

template <typename R = void>
#ifndef _DOXYGEN_
requires (!std::is_reference_v<R>)
#endif
class task;

template <typename R = void>
class coroutine;

struct job;

#ifdef DPP_CORO_TEST
/**
 * @brief Allocation count of a certain type, for testing purposes.
 *
 * @todo Remove when coro is stable
 */
template <typename T>
inline int coro_alloc_count = 0;
#endif

} // namespace dpp

#endif /* DPP_CORO */

