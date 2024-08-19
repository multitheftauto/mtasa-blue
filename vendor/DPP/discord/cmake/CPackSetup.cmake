include(GNUInstallDirs)
set(DPP_EXPORT_NAME dpp)
set(DPP_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${DPP_EXPORT_NAME})
set(DPP_VERSION_FILE ${PROJECT_BINARY_DIR}/${DPP_EXPORT_NAME}-config-version.cmake)

# The three lines below are only used for windows builds
set(DPP_VERSIONED ${DPP_EXPORT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
set(DPP_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${DPP_VERSIONED})
set(DPP_INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR}/${DPP_VERSIONED})


## Pack the binary output
if (WIN32)
	install(TARGETS dpp
		EXPORT ${DPP_EXPORT_NAME}
		LIBRARY DESTINATION  ${DPP_INSTALL_LIBRARY_DIR}
		ARCHIVE DESTINATION  ${DPP_INSTALL_LIBRARY_DIR}
		RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
		INCLUDES DESTINATION ${DPP_INSTALL_INCLUDE_DIR})
	install(DIRECTORY "${DPP_ROOT_PATH}/include/" DESTINATION "${DPP_INSTALL_INCLUDE_DIR}")
else()
	install(TARGETS dpp
		EXPORT ${DPP_EXPORT_NAME}
		LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBRARY_DIR}
		ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBRARY_DIR}
		RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR})
endif()

## Allow for a specific version to be chosen in the `find_package` command
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${DPP_VERSION_FILE}
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion)

## Include the file which allows `find_package(dpp)` to function.
install(FILES "${DPP_ROOT_PATH}/cmake/dpp-config.cmake" "${DPP_VERSION_FILE}" DESTINATION "${DPP_CMAKE_DIR}")

## Export the targets to allow other projects to easily include this project
install(EXPORT "${DPP_EXPORT_NAME}" DESTINATION "${DPP_CMAKE_DIR}" NAMESPACE dpp::)

# Prepare information for packaging into .zip, .deb, .rpm
## Project installation metadata
set(CPACK_PACKAGE_NAME   libdpp)	# Name of generated file
set(CPACK_PACKAGE_VENDOR Brainbox.cc)	# Maker of the application
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An incredibly lightweight C++ Discord library")
set(CPACK_PACKAGE_DESCRIPTION "An incredibly lightweight C++ Discord library")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://dpp.dev/")
set(CPACK_FREEBSD_PACKAGE_MAINTAINER "bsd@dpp.dev")
set(CPACK_FREEBSD_PACKAGE_ORIGIN "misc/libdpp")
set(CPACK_RPM_PACKAGE_LICENSE "Apache 2.0")
set(CPACK_PACKAGE_CONTACT "https://discord.gg/dpp") # D++ Development Discord
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsodium23 (>= 1.0.17-1), libopus0 (>= 1.3-1)")
set(CPACK_RPM_PACKAGE_REQUIRES "libsodium >= 1.0.17, opus >= 1.3.1")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "An incredibly lightweight C++ Discord library")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")

## Select generated based on what operating system
if(WIN32)
	set(CPACK_GENERATOR ZIP)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CPACK_GENERATOR "DEB;RPM")
else()
	set(CPACK_GENERATOR "TBZ2")
endif()
