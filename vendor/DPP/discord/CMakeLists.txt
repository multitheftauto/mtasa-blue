#
# D++ (DPP), The Lightweight C++ Discord Library
#
# Copyright 2021 Craig Edwards <support@brainbox.cc>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required (VERSION 3.16)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_VOICE_SUPPORT "Build voice support" ON)
option(RUN_LDCONFIG "Run ldconfig after installation" ON)
option(DPP_INSTALL "Generate the install target" ON)
option(DPP_BUILD_TEST "Build the test program" ON)
option(DPP_NO_VCPKG "No VCPKG" OFF)
option(DPP_CORO "Experimental support for C++20 coroutines" OFF)
option(DPP_USE_EXTERNAL_JSON "Use an external installation of nlohmann::json" OFF)
option(DPP_USE_PCH "Use precompiled headers to speed up compilation" OFF)
option(AVX_TYPE "Force AVX type for speeding up audio mixing" OFF)

include(CheckCXXSymbolExists)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(DPP_BUILD)

set(DPP_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(READ "${DPP_ROOT_PATH}/include/dpp/version.h" version_h)

if(NOT version_h MATCHES "DPP_VERSION_SHORT ([0-9][0-9])([0-9][0-9])([0-9][0-9])")
	message(FATAL_ERROR "Cannot get DPP_VERSION_SHORT from version.h")
endif()

math(EXPR DPP_VERSION_MAJOR "${CMAKE_MATCH_1}")
math(EXPR DPP_VERSION_MINOR "${CMAKE_MATCH_2}")
math(EXPR DPP_VERSION_PATCH "${CMAKE_MATCH_3}")

string(CONCAT DPP_VERSION "${DPP_VERSION_MAJOR}.${DPP_VERSION_MINOR}.${DPP_VERSION_PATCH}")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${DPP_ROOT_PATH}/cmake/")

if (DPP_NO_VCPKG)
	message("-- INFO: Explicitly disabling VCPKG as running inside the CI action.")
else()
	message("-- INFO: Using VCPKG if detected")
endif()

if (WIN32 AND NOT MINGW AND BUILD_SHARED_LIBS)
	message("-- INFO: Configuring .rc resource script")
	configure_file("${DPP_ROOT_PATH}/src/dpp/dpp.rc.in" "${DPP_ROOT_PATH}/src/dpp/dpp.rc" NEWLINE_STYLE WIN32)
endif()

if (NOT DPP_NO_VCPKG AND EXISTS "${_VCPKG_ROOT_DIR}")
	set(PROJECT_NAME "dpp")
	project(
		"${PROJECT_NAME}"
		VERSION "${DPP_VERSION}"
		LANGUAGES CXX
		HOMEPAGE_URL "https://dpp.dev/"
		DESCRIPTION "An incredibly lightweight C++ Discord library."
	)

	if (MSVC AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(DPP_CLANG_CL true)
	endif()

	# Required before we add any subdirectories.
	if (DPP_BUILD_TEST)
		enable_testing(${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	add_subdirectory(library-vcpkg)
else()
	set(PROJECT_NAME "libdpp")
	project(
		"${PROJECT_NAME}"
		VERSION "${DPP_VERSION}"
		LANGUAGES CXX
		HOMEPAGE_URL "https://dpp.dev/"
		DESCRIPTION "An incredibly lightweight C++ Discord library."
	)

	if (MSVC AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(DPP_CLANG_CL true)
	endif()

	# Required before we add any subdirectories.
	if (DPP_BUILD_TEST)
		enable_testing(${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	add_subdirectory(library)
endif()

if(DPP_USE_EXTERNAL_JSON)
	# We do nothing here, we just assume it is on the include path.
	# nlohmann::json's cmake stuff does all kinds of weird, and is more hassle than it's worth.
	# This functionality is here mostly for package maintainers so if you enable it you should
	# know what you are doing.
	message("-- Using external nlohmann::json")
	target_compile_definitions(dpp PUBLIC DPP_USE_EXTERNAL_JSON)
else()
	# Add the nlohmann single include to the include path. Note that nlohmann::json is kinda
	# fussy, this is an older version because trying to use v3.11.2 gave a bunch of parse errors
	# that made no sense, it seems they may have changed their parsing rules somehow.
	message("-- Using bundled nlohmann::json")
endif()
