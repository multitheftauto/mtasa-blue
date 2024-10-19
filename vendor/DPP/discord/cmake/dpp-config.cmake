# dpp-config.cmake - package configuration file

## Get current filesystem path (will a prefixed by where this package was installed)
get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

## Use this directory to include dpp which has the rest of the project targets
include(${SELF_DIR}/dpp.cmake)

## Set OpenSSl directory for macos. It is also in our main CMakeLists.txt, but this file is independent from that.
if(APPLE)
	if(CMAKE_APPLE_SILICON_PROCESSOR)
		set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
	else()
		set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
	endif()
	find_package(OpenSSL REQUIRED)
endif()

# Search for libdpp dependencies
include(CMakeFindDependencyMacro)
find_dependency(OpenSSL REQUIRED)
