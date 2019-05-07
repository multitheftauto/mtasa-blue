cmake_minimum_required(VERSION 3.2)

project(rectpack2D)

set(RESULT_EXE_WORKING_DIR "${PROJECT_SOURCE_DIR}/example")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	message("Building with Clang (${CMAKE_CXX_COMPILER}).")
	set(CLANG ON)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	message("Building with gcc (${CMAKE_CXX_COMPILER}).")
	set(GCC ON)
elseif(MSVC)
	message("Building with MSVC.")
else()
	message("Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if(UNIX)
	message("Building rectpack2D for UNIX systems.")
elseif(MSVC)
	message("Building rectpack2D for Windows systems.")
else()
	message("Unknown system.")
endif()
  
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

if(UNIX)
	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} -std=gnu++1z"
	)
elseif(MSVC)
	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} /Ob2 /GL /Oi /O2 /permissive- /std:c++latest"
	)
	
	set(CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} /LTCG"
	)
endif()

if (CLANG)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld ")

	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -ftemplate-backtrace-limit=0 -fcolor-diagnostics -std=gnu++1z -stdlib=libc++ -Wno-error=unused-command-line-argument -Wno-error=missing-braces"
	)
endif()

add_executable(rectpack2D "example/main.cpp")

if (CLANG)
	target_link_libraries(rectpack2D c++experimental)
elseif(GCC)
	target_link_libraries(rectpack2D stdc++fs)
endif()

if(MSVC)
	set_target_properties(rectpack2D 
		PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${RESULT_EXE_WORKING_DIR}
	)

	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT rectpack2D)
endif()

if(UNIX)
	add_custom_target(run
		COMMAND rectpack2D
		DEPENDS rectpack2D
		WORKING_DIRECTORY ${RESULT_EXE_WORKING_DIR} 
	)
endif()
