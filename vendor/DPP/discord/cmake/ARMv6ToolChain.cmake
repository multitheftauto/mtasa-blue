SET(CMAKE_SYSTEM_NAME Linux)
# Possibly needed tweak
#SET(CMAKE_SYSTEM_PROCESSOR armhf)

#If you have installed cross compiler to somewhere else, please specify that path.
SET(COMPILER_ROOT /opt/cross-pi-gcc)

SET(CMAKE_C_COMPILER ${COMPILER_ROOT}/bin/arm-linux-gnueabihf-gcc-8.3.0)
SET(CMAKE_CXX_COMPILER ${COMPILER_ROOT}/bin/arm-linux-gnueabihf-g++)

# Below call is necessary to avoid non-RT problem.
SET(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
SET(CPACK_RPM_PACKAGE_ARCHITECTURE armhf)

SET(RASPBERRY_ROOT_PATH ${CMAKE_CURRENT_LIST_DIR}/arm_raspberry)
SET(RASPBERRY_KINETIC_PATH ${RASPBERRY_ROOT_PATH}/opt/ros/kinetic)

SET(CMAKE_FIND_ROOT_PATH ${RASPBERRY_ROOT_PATH} ${CATKIN_DEVEL_PREFIX})


#Have to set this one to BOTH, to allow CMake to find rospack
#This set of variables controls whether the CMAKE_FIND_ROOT_PATH and CMAKE_SYSROOT are used for find_xxx() operations.
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
#SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

SET(CMAKE_PREFIX_PATH ${RASPBERRY_KINETIC_PATH} ${RASPBERRY_ROOT_PATH}/usr)

UNSET(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES)
UNSET(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)

SET(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)
INCLUDE_DIRECTORIES(
	${COMPILER_ROOT}/arm-linux-gnueabihf/libc/usr/include
	${COMPILER_ROOT}/arm-linux-gnueabihf/include
	${COMPILER_ROOT}/arm-linux-gnueabihf/include/c++/8.3.0
	${COMPILER_ROOT}/arm-linux-gnueabihf/include/c++/8.3.0/arm-linux-gnueabihf
	${COMPILER_ROOT}/lib/gcc/arm-linux-gnueabihf/8.3.0/include
	${COMPILER_ROOT}/lib/gcc/arm-linux-gnueabihf/8.3.0/include-fixed
	${DPP_ROOT_PATH}/rootfs/usr/include/arm-linux-gnueabihf)
SET(CMAKE_INCLUDE_DIRECTORIES_BEFORE OFF)

SET(ZLIB_LIBRARY ${DPP_ROOT_PATH}/rootfs/lib/arm-linux-gnueabihf/libz.so.1.2.11)
SET(OPENSSL_CRYPTO_LIBRARY ${DPP_ROOT_PATH}/rootfs/usr/lib/arm-linux-gnueabihf/libcrypto.so.1.1)
SET(OPENSSL_SSL_LIBRARY ${DPP_ROOT_PATH}/rootfs/usr/lib/arm-linux-gnueabihf/libssl.so.1.1)

SET(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -nostdinc --sysroot=${RASPBERRY_ROOT_PATH} -Wno-psabi " CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -nostdinc -nostdinc++ --sysroot=${RASPBERRY_ROOT_PATH} -Wno-psabi " CACHE INTERNAL "" FORCE)
SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -ldl" CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot=${RASPBERRY_ROOT_PATH} -ldl" CACHE INTERNAL "" FORCE)

SET(LD_LIBRARY_PATH ${RASPBERRY_KINETIC_PATH}/lib)

EXECUTE_PROCESS(COMMAND wget -P ${DPP_ROOT_PATH}/rootfs -q http://content.dpp.dev/zlib1g_1.2.11.dfsg-1_armhf.deb http://content.dpp.dev/zlib1g-dev_1.2.11.dfsg-1_armhf.deb http://content.dpp.dev/libssl1.1_1.1.1m-1_armhf.deb http://content.dpp.dev/libssl-dev_1.1.1m-1_armhf.deb https://content.dpp.dev/raspi-toolchain.tar.gz)

EXECUTE_PROCESS(
	COMMAND tar -xzf ${DPP_ROOT_PATH}/rootfs/raspi-toolchain.tar.gz -C /opt
	COMMAND sudo dpkg-deb -x ${DPP_ROOT_PATH}/rootfs/zlib1g-dev_1.2.11.dfsg-1_armhf.deb ${DPP_ROOT_PATH}/rootfs
	COMMAND sudo dpkg-deb -x ${DPP_ROOT_PATH}/rootfs/zlib1g_1.2.11.dfsg-1_armhf.deb ${DPP_ROOT_PATH}/rootfs
	COMMAND sudo dpkg-deb -x ${DPP_ROOT_PATH}/rootfs/libssl-dev_1.1.1m-1_armhf.deb ${DPP_ROOT_PATH}/rootfs
	COMMAND sudo dpkg-deb -x ${DPP_ROOT_PATH}/rootfs/libssl1.1_1.1.1m-1_armhf.deb ${DPP_ROOT_PATH}/rootfs)
