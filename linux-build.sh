#!/bin/bash -e

# Set variable defaults
: ${BUILD_CONFIG:=release}
: ${PREMAKE_FILE:=premake5.lua}

if [ "$(uname)" == "Darwin" ]; then
    cores=$(sysctl -n hw.ncpu)
    : ${NUM_CORES:=$cores}
    : ${PREMAKE5:=utils/premake5-macos}
    : ${BUILD_OS:=macosx}
    : ${BUILD_ARCHITECTURE:=arm64}
    : ${AR:=ar}
    : ${CC:=clang}
    : ${CXX:=clang++}
else
    cores=$(grep -c ^processor /proc/cpuinfo)
    : ${NUM_CORES:=$cores}
    : ${PREMAKE5:=utils/premake5}
    : ${BUILD_OS:=linux}
    : ${BUILD_ARCHITECTURE:=x64}
    : ${AR:=ar}
    : ${CC:=gcc}
    : ${CXX:=g++}
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
    ;;
    64|x64)
        CONFIG=${BUILD_CONFIG}_x64
    ;;
    arm)
        CONFIG=${BUILD_CONFIG}_${BUILD_ARCHITECTURE}
    ;;
    arm64)
        CONFIG=${BUILD_CONFIG}_${BUILD_ARCHITECTURE}
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
