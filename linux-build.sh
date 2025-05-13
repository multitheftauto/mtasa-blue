#!/bin/bash -e

# Set variable defaults
: ${BUILD_OS:=linux}
: ${BUILD_ARCHITECTURE:=x64}
: ${BUILD_CONFIG:=release}
: ${PREMAKE_FILE:=premake5.lua}

# Find premake binary location
if [ "$(uname)" == "Darwin" ]; then
    PREMAKE5=utils/premake5-macos
else
    PREMAKE5=utils/premake5
fi

# Number of cores
if [ "$(uname)" == "Darwin" ]; then
    NUM_CORES=$(sysctl -n hw.ncpu)
    : ${GCC_PREFIX:=}
    : ${AR:=ar}
    : ${CC:=gcc}
    : ${CXX:=g++}
else
    NUM_CORES=$(grep -c ^processor /proc/cpuinfo)
fi

# Read script arguments
while [ $# -gt 0 ]; do
    case "$1" in
        --os=*)     BUILD_OS="${1#*=}"              ;;
        --arch=*)   BUILD_ARCHITECTURE="${1#*=}"    ;;
        --config=*) BUILD_CONFIG="${1#*=}"          ;;
        --cores=*)  NUM_CORES="${1#*=}"             ;;
        --file=*)   PREMAKE_FILE="${1#*=}"          ;;
        *)
            echo "Error: Invalid argument: $1" >&2
            exit 1
    esac
    shift
done

# Display script arguments
echo "Build configuration:"
echo "  BUILD_ARCHITECTURE = $BUILD_ARCHITECTURE"
echo "  BUILD_CONFIG = $BUILD_CONFIG"

# Verify script arguments
case $BUILD_CONFIG in
    debug|release) ;;
    *)
        echo "Error: Invalid build configuration" >&2
        exit 1
esac

case $BUILD_ARCHITECTURE in
    32|x86)
        CONFIG=${BUILD_CONFIG}_x86
        : ${GCC_PREFIX:=i386-linux-gnu-}
        : ${AR:=x86_64-linux-gnu-gcc-ar-10}
        : ${CC:=x86_64-linux-gnu-gcc-10}
        : ${CXX:=x86_64-linux-gnu-g++-10}
    ;;
    64|x64)
        CONFIG=${BUILD_CONFIG}_x64
        : ${GCC_PREFIX:=x86_64-linux-gnu-}
        : ${AR:=x86_64-linux-gnu-gcc-ar-10}
        : ${CC:=x86_64-linux-gnu-gcc-10}
        : ${CXX:=x86_64-linux-gnu-g++-10}
    ;;
    arm)
        CONFIG=${BUILD_CONFIG}_${BUILD_ARCHITECTURE}
        : ${GCC_PREFIX:=arm-linux-gnueabihf-}
        : ${AR:=arm-linux-gnueabihf-ar}
        : ${CC:=arm-linux-gnueabihf-gcc-10}
        : ${CXX:=arm-linux-gnueabihf-g++-10}
    ;;
    arm64)
        CONFIG=${BUILD_CONFIG}_${BUILD_ARCHITECTURE}
        : ${GCC_PREFIX:=aarch64-linux-gnu-}
        : ${AR:=aarch64-linux-gnu-gcc-ar-10}
        : ${CC:=aarch64-linux-gnu-gcc-10}
        : ${CXX:=aarch64-linux-gnu-g++-10}
    ;;
    *)
        echo "Error: Invalid build architecture" >&2
        exit 1
esac

echo "  OS = $BUILD_OS"
echo "  CONFIG = $CONFIG"
echo "  AR = $AR"
echo "  CC = $CC"
echo "  CXX = $CXX"

# Clean old build files
rm -Rf Build/
rm -Rf Bin/

# Generate Makefiles
if [[ -n "$GCC_PREFIX" ]]; then
    $PREMAKE5 --gccprefix="$GCC_PREFIX" --os="$BUILD_OS" --file=$PREMAKE_FILE gmake
else
    $PREMAKE5 --os="$BUILD_OS" --file=$PREMAKE_FILE gmake
fi

# Build!
make -C Build/ -j ${NUM_CORES} AR=${AR} CC=${CC} CXX=${CXX} config=${CONFIG} all
