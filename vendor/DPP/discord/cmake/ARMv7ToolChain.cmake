SET(CMAKE_SYSTEM_NAME Linux)
# Possibly needed tweak
#SET(CMAKE_SYSTEM_PROCESSOR armhf)

SET(CMAKE_C_COMPILER ${COMPILER_ROOT}/arm-linux-gnueabihf-gcc-8)
SET(CMAKE_CXX_COMPILER ${COMPILER_ROOT}/arm-linux-gnueabihf-g++-8)

# Below call is necessary to avoid non-RT problem.
SET(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
SET(CPACK_RPM_PACKAGE_ARCHITECTURE armhf)

SET(RASPBERRY_ROOT_PATH ${DPP_ROOT_PATH}/arm_raspberry)
SET(RASPBERRY_KINETIC_PATH ${RASPBERRY_ROOT_PATH}/opt/ros/kinetic)

SET(CMAKE_FIND_ROOT_PATH ${RASPBERRY_ROOT_PATH} ${CATKIN_DEVEL_PREFIX})

#If you have installed cross compiler to somewhere else, please specify that path.
SET(COMPILER_ROOT /usr/bin)

#Have to set this one to BOTH, to allow CMake to find rospack
#This set of variables controls whether the CMAKE_FIND_ROOT_PATH and CMAKE_SYSROOT are used for find_xxx() operations.
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
#SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

INCLUDE_DIRECTORIES(
	/usr/include/arm-linux-gnueabihf)

SET(ZLIB_LIBRARY /lib/arm-linux-gnueabihf/libz.so.1.2.11)
SET(OPENSSL_CRYPTO_LIBRARY /usr/lib/arm-linux-gnueabihf/libcrypto.so.1.1)
SET(OPENSSL_SSL_LIBRARY /usr/lib/arm-linux-gnueabihf/libssl.so.1.1)

SET(CMAKE_PREFIX_PATH ${RASPBERRY_KINETIC_PATH} ${RASPBERRY_ROOT_PATH}/usr)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -Wno-psabi" CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -Wno-psabi" CACHE INTERNAL "" FORCE)
SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -ldl" CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -ldl" CACHE INTERNAL "" FORCE)

SET(LD_LIBRARY_PATH ${RASPBERRY_KINETIC_PATH}/lib)

EXECUTE_PROCESS(COMMAND printf "deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ focal main multiverse restricted universe\ndeb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports/ focal main multiverse restricted universe\ndeb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports/ focal-updates main multiverse restricted universe\ndeb [arch=amd64] http://archive.ubuntu.com/ubuntu/ focal-updates main multiverse restricted universe\ndeb [arch=amd64] http://security.ubuntu.com/ubuntu/ focal-security main multiverse restricted universe"
	OUTPUT_FILE TMPFILE)
EXECUTE_PROCESS(COMMAND sudo mv TMPFILE /etc/apt/sources.list)
EXECUTE_PROCESS(COMMAND sudo dpkg --add-architecture armhf)
EXECUTE_PROCESS(COMMAND sudo apt update)
EXECUTE_PROCESS(COMMAND sudo apt install -y cmake gcc-8-arm-linux-gnueabihf g++-8-arm-linux-gnueabihf zlib1g-dev:armhf libssl-dev:armhf libopus-dev:armhf libsodium-dev:armhf)
EXECUTE_PROCESS(COMMAND sudo mv /usr/lib/arm-linux-gnueabihf/pkgconfig/libsodium.pc /usr/lib/pkgconfig/)

