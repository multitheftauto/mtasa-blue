SET(CMAKE_SYSTEM_NAME Linux)
# Possibly needed tweak
#SET(CMAKE_SYSTEM_PROCESSOR aarch64)

SET(CMAKE_C_COMPILER ${COMPILER_ROOT}/aarch64-linux-gnu-gcc-8)
SET(CMAKE_CXX_COMPILER ${COMPILER_ROOT}/aarch64-linux-gnu-g++-8)

# Below call is necessary to avoid non-RT problem.
SET(CMAKE_LIBRARY_ARCHITECTURE aarch64-linux-gnu)
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE aarch64)
SET(CPACK_RPM_PACKAGE_ARCHITECTURE aarch64)

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
	/usr/include/aarch64-linux-gnu)

SET(ZLIB_LIBRARY /lib/aarch64-linux-gnu/libz.so.1.2.11)
SET(OPENSSL_CRYPTO_LIBRARY /usr/lib/aarch64-linux-gnu/libcrypto.so)
SET(OPENSSL_SSL_LIBRARY /usr/lib/aarch64-linux-gnu/libssl.so)

SET(CMAKE_PREFIX_PATH ${RASPBERRY_KINETIC_PATH} ${RASPBERRY_ROOT_PATH}/usr)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -Wno-psabi" CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -Wno-psabi" CACHE INTERNAL "" FORCE)
SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -ldl" CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -ldl" CACHE INTERNAL "" FORCE)

SET(LD_LIBRARY_PATH ${RASPBERRY_KINETIC_PATH}/lib)

EXECUTE_PROCESS(COMMAND printf "deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ focal main multiverse restricted universe\ndeb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports/ focal main multiverse restricted universe\ndeb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports/ focal-updates main multiverse restricted universe\ndeb [arch=amd64] http://archive.ubuntu.com/ubuntu/ focal-updates main multiverse restricted universe\ndeb [arch=amd64] http://security.ubuntu.com/ubuntu/ focal-security main multiverse restricted universe" 
	OUTPUT_FILE TMPFILE)
EXECUTE_PROCESS(COMMAND sudo mv TMPFILE /etc/apt/sources.list)
EXECUTE_PROCESS(COMMAND sudo dpkg --add-architecture arm64)
EXECUTE_PROCESS(COMMAND sudo apt-add-repository -y ppa:canonical-kernel-team/ppa)
EXECUTE_PROCESS(COMMAND sudo apt update)
EXECUTE_PROCESS(COMMAND sudo apt install -y cmake gcc-8-aarch64-linux-gnu g++-8-aarch64-linux-gnu libc6-dev-arm64-cross zlib1g-dev:arm64 libssl-dev:arm64 libopus-dev:arm64 libsodium-dev:arm64)
EXECUTE_PROCESS(COMMAND sudo mv /usr/lib/aarch64-linux-gnu/pkgconfig/libsodium.pc /usr/lib/pkgconfig/)

