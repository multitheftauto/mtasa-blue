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

/* Compile-time check for C++17.
 * Either one of the following causes a compile time error:
 * __cplusplus not defined at all (this means we are being compiled on a C compiler)
 * MSVC defined and _MSVC_LANG < 201703L (Visual Studio, but not C++17 or newer)
 * MSVC not defined and __cplusplus < 201703L (Non-visual studio, but not C++17 or newer)
 * The additional checks are required because MSVC doesn't correctly set __cplusplus to 201703L,
 * which is hugely non-standard, but apparently "it broke stuff" so they dont ever change it
 * from C++98. Ugh.
 */
#if (!defined(__cplusplus) || (defined(_MSC_VER) && (!defined(_MSVC_LANG) || _MSVC_LANG < 201703L)) || (!defined(_MSC_VER) && __cplusplus < 201703L))
	#error "D++ Requires a C++17 compatible C++ compiler. Please ensure that you have enabled C++17 in your compiler flags."
#endif

#ifndef DPP_STATIC
	/* Dynamic linked build as shared object or dll */
	#ifdef DPP_BUILD
		/* Building the library */
		#ifdef _WIN32
			#include <dpp/win32_safe_warnings.h>
			#define DPP_EXPORT __declspec(dllexport)
		#else
			#define DPP_EXPORT
		#endif
	#else
		/* Including the library */
		#ifdef _WIN32
			#define DPP_EXPORT __declspec(dllimport)
		#else
			#define DPP_EXPORT
		#endif
	#endif
#else
	/* Static linked build */
	#if defined(_WIN32) && defined(DPP_BUILD)
		#include <dpp/win32_safe_warnings.h>
	#endif
	#define DPP_EXPORT
#endif

namespace dpp {

/**
 * @brief Represents a build configuration. On some platforms (e.g. Windows) release isn't compatible with debug, so we use this enum to detect it.
 */
enum class build_type {
	/**
	 * @brief Universal build, works with both debug and release
	 */
	universal,

	/**
	 * @brief Debug build
	 */
	debug,

	/**
	 * @brief Release build
	 */
	release
};

template <build_type>
extern bool DPP_EXPORT validate_configuration();

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT) || defined(UE_BUILD_TEST) || defined(UE_BUILD_SHIPPING) || defined(UE_GAME) || defined(UE_EDITOR) || defined(UE_BUILD_SHIPPING_WITH_EDITOR) || defined(UE_BUILD_DOCS)
	/*
	 * We need to tell DPP to NOT do the version checker if something from Unreal Engine is defined.
	 * We have to do this because UE is causing some weirdness where the version checker is broken and always errors.
	 * This is really only for DPP-UE. There is no reason to not do the version checker unless you are in Unreal Engine.
	 */
	#define DPP_BYPASS_VERSION_CHECKING
#endif /* UE */

#ifndef DPP_BUILD /* when including dpp */
	/**
	 * Version checking, making sure the program is in a configuration compatible with DPP's.
	 *
	 * Do NOT make these variables constexpr.
	 * We want them to initialize at runtime so the function can be pulled from the shared library object.
	 */
	#ifndef DPP_BYPASS_VERSION_CHECKING
		#if defined(_WIN32)
			#ifdef _DEBUG
				inline const bool is_valid_config = validate_configuration<build_type::debug>();
			#else
				inline const bool is_valid_config = validate_configuration<build_type::release>();
			#endif /* _DEBUG */
		#else
			inline const bool is_valid_config = validate_configuration<build_type::universal>();
		#endif /* _WIN32 */
	#endif /* !DPP_BYPASS_VERSION_CHECKING */
#endif /* !DPP_BUILD */

}

#ifndef _WIN32
	#ifndef SOCKET
		#define SOCKET int
	#endif
#else
    #ifndef NOMINMAX
		#define NOMINMAX
	#endif

	#include <WinSock2.h>
#endif

#ifdef _DOXYGEN_
	/** @brief Macro that expands to [[deprecated(reason)]] when including the library, nothing when building the library */
	#define DPP_DEPRECATED(reason)
#else /* !_DOXYGEN_ */
	#if defined(DPP_BUILD) || defined(DPP_NO_DEPRECATED)
		/** @brief Macro that expands to [[deprecated(reason)]] when including the library, nothing when building the library */
		#define DPP_DEPRECATED(reason)
	#else
		/** @brief Macro that expands to [[deprecated(reason)]] when including the library, nothing when building the library */
		#define DPP_DEPRECATED(reason) [[deprecated(reason)]]
	#endif
#endif /* _DOXYGEN_ */
