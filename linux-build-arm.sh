#!/bin/bash -e

BUILD_ARCHITECTURE=arm64
BUILD_CONFIG=release
NUM_CORES=$(grep -c ^processor /proc/cpuinfo)

while [ $# -gt 0 ]; do
    case "$1" in
        --arch=*)
            BUILD_ARCHITECTURE="${1#*=}"
        ;;
        --config=*)
            BUILD_CONFIG="${1#*=}"
        ;;
        *)
            printf "Error: Invalid argument: $1\n"
            exit 1
    esac
    shift
done

echo "BUILD_ARCHITECTURE = $BUILD_ARCHITECTURE"
echo "BUILD_CONFIG = $BUILD_CONFIG"

case $BUILD_ARCHITECTURE in
    arm64)
        GCC_PREFIX=aarch64-linux-gnu-
    ;;
    armhf)
        GCC_PREFIX=arm-linux-gnueabihf-
    ;;
    *)
        printf "Error: Invalid build architecture\n"
        exit 1
esac

echo "GCC_PREFIX = $GCC_PREFIX"

# Clean old build files
rm -Rf Build/
rm -Rf Bin/

# Generate makefiles
utils/premake5 --arch=$BUILD_ARCHITECTURE --gccprefix=$GCC_PREFIX gmake

# Remove -m32, -m64, -msse2 from *FLAGS
sed -i '/FLAGS +=/ s/-m32//i;/FLAGS +=/ s/-msse2//i;/FLAGS +=/ s/-m64//i' Build/*.make

# Build!
make -C Build/ -j $NUM_CORES \
    AR=/bin/${GCC_PREFIX}ar \
    CC=/bin/${GCC_PREFIX}gcc-10 \
    CXX=/bin/${GCC_PREFIX}g++-10 \
    config=debug_x64 \
    all
