message("-- Building for windows (x86) with precompiled packaged dependencies")

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(ZLIB_LIBRARIES "${PROJECT_SOURCE_DIR}/win32/32/lib")
set(ZLIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/win32/include")
set(OPENSSL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/win32/include")
set(OPENSSL_LIBRARIES "${PROJECT_SOURCE_DIR}/win32/32/lib")
ADD_DEFINITIONS(/bigobj)

link_libraries("${PROJECT_SOURCE_DIR}/win32/32/lib/libssl.lib")
link_libraries("${PROJECT_SOURCE_DIR}/win32/32/lib/libcrypto.lib")
link_libraries("${PROJECT_SOURCE_DIR}/win32/32/lib/zlib.lib")
link_libraries("${PROJECT_SOURCE_DIR}/win32/32/lib/libsodium.lib")
link_libraries("${PROJECT_SOURCE_DIR}/win32/32/lib/opus.lib")

set(OPUS_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/win32/include")
set(OPUS_LIBRARIES "${PROJECT_SOURCE_DIR}/win32/32/lib/opus.lib")
set(sodium_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/win32/include")
set(sodium_LIBRARY_DEBUG "${PROJECT_SOURCE_DIR}/win32/32/lib/libsodium.lib")
set(sodium_LIBRARY_RELEASE "${PROJECT_SOURCE_DIR}/win32/32/lib/libsodium.lib")
set(HAVE_OPUS_OPUS_H "${PROJECT_SOURCE_DIR}/win32/include/opus/opus.h")
set(OPUS_FOUND 1)
SET(sodium_VERSION_STRING "win32 bundled")

include_directories("${PROJECT_SOURCE_DIR}/win32/include")

add_compile_definitions(OPENSSL_SYS_WIN32)
add_compile_definitions(_WINSOCK_DEPRECATED_NO_WARNINGS)
add_compile_definitions(WIN32_LEAN_AND_MEAN)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)

SET(WINDOWS_32_BIT 1)

# BIG FAT STINKY KLUDGE
SET(CMAKE_CXX_COMPILER_WORKS 1)
