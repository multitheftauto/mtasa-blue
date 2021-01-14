cmake_minimum_required(VERSION 3.3)

project(lunasvg VERSION 1.4.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

option(LUNASVG_BUILD_EXAMPLES "Builds examples" OFF)

add_library(lunasvg STATIC)

add_subdirectory(include)
add_subdirectory(source)
add_subdirectory(3rdparty/stb)
add_subdirectory(3rdparty/software)
add_subdirectory(3rdparty/plutovg)

if(LUNASVG_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

set(LUNASVG_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set(LUNASVG_INCDIR ${CMAKE_INSTALL_PREFIX}/include/lunasvg)

install(FILES
    include/svgdocument.h
    include/svgelement.h
    include/svgelementiter.h
    DESTINATION ${LUNASVG_INCDIR}
)

install(TARGETS lunasvg
    LIBRARY     DESTINATION    ${LUNASVG_LIBDIR}
    ARCHIVE     DESTINATION    ${LUNASVG_LIBDIR}
    INCLUDES    DESTINATION    ${LUNASVG_INCDIR}
)
