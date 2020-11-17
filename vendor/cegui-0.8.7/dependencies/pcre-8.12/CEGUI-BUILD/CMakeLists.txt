set(DEPNAME pcre)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../pcre_*.c")
set(HEADERS pcre.h)

include_directories(../ ${CMAKE_BINARY_DIR}/dependencies/include)

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../pcre.h")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../pcre.h.generic"
                                                     "${CMAKE_CURRENT_SOURCE_DIR}/../pcre.h")
endif()
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../config.h")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../config.h.generic"
                                                     "${CMAKE_CURRENT_SOURCE_DIR}/../config.h")
endif()

list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/../pcre_chartables.c")
cegui_add_library(${DEPNAME} SOURCES NONE)
if(CMAKE_CROSSCOMPILING)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../pcre_chartables.c.dist
                   "${CMAKE_CURRENT_BINARY_DIR}/../pcre_chartables.c"
                   COPYONLY)
else()
    add_executable(dftables ${CMAKE_CURRENT_SOURCE_DIR}/../dftables.c)
    set_target_properties(dftables PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.."
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/.."
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/.."
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_BINARY_DIR}/.."
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_BINARY_DIR}/.."
        COMPILE_DEFINITIONS "HAVE_CONFIG_H;HAVE_STDINT_H=0;HAVE_INTTYPES_H=0")
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/../pcre_chartables.c"
                       COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../dftables" "${CMAKE_CURRENT_BINARY_DIR}/../pcre_chartables.c")
    add_dependencies(${DEPNAME} dftables)
    add_dependencies(${DEPNAME}_static dftables)
endif()

set_target_properties(${DEPNAME} PROPERTIES
    COMPILE_DEFINITIONS "HAVE_WINDOWS_H;HAVE_ZLIB_H;SUPPORT_UTF8;HAVE_CONFIG_H;HAVE_STDINT_H=0;HAVE_INTTYPES_H=0")

target_link_libraries(${DEPNAME} zlib)

# this is needed because the headers for zlib are only copied as part of
# the shared build
add_dependencies(${DEPNAME}_static zlib)

set_target_properties(${DEPNAME}_static PROPERTIES
    COMPILE_DEFINITIONS "PCRE_STATIC;HAVE_WINDOWS_H;HAVE_ZLIB_H;SUPPORT_UTF8;HAVE_CONFIG_H;HAVE_STDINT_H=0;HAVE_INTTYPES_H=0")

foreach(_H_FILE ${HEADERS})
    add_custom_command(TARGET ${DEPNAME} POST_BUILD 
                       COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../${_H_FILE}"
                                                        "${CMAKE_BINARY_DIR}/dependencies/include/${_H_FILE}")
endforeach()

add_custom_command(TARGET ${DEPNAME} POST_BUILD 
                   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../LICENCE"
                                                    "${CMAKE_BINARY_DIR}/dependencies/pcre-license.txt")
