#!/bin/bash -e

# Set variable defaults
: ${BUILD_OS:=linux}
: ${BUILD_ARCHITECTURE:=x64}
: ${BUILD_CONFIG:=release}

# Find premake binary location
if [ "$(uname)" == "Darwin" ]; then
    PREMAKE5=utils/premake5-macos
else
    PREMAKE5=utils/premake5
fi

# Number of cores
if [ "$(uname)" == "Darwin" ]; then
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    NUM_CORES=$(grep -c ^processor /proc/cpuinfo)
fi

# Read script arguments
while [ $# -gt 0 ]; do
    case "$1" in
        --os=*)     BUILD_OS="${1#*=}"              ;;
        --arch=*)   BUILD_ARCHITECTURE="${1#*=}"    ;;
        --config=*) BUILD_CONFIG="${1#*=}"          ;;
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
    arm64|arm)
        CONFIG=${BUILD_CONFIG}_${BUILD_ARCHITECTURE}
    ;;
    *)
        echo "Error: Invalid build architecture" >&2
        exit 1
esac

echo "  OS = $BUILD_OS"
echo "  CONFIG = $CONFIG"
echo "  AR = ${AR:=ar}"
echo "  CC = ${CC:=gcc}"
echo "  CXX = ${CXX:=g++}"

# Clean old build files
rm -Rf Build/
rm -Rf Bin/

# Generate Makefiles
if [[ -n "$GCC_PREFIX" ]]; then
    $PREMAKE5 --gccprefix="$GCC_PREFIX" --os="$BUILD_OS" gmake
else
    $PREMAKE5 --os="$BUILD_OS" gmake
fi

# Build!
make -C Build/ -j ${NUM_CORES} AR=${AR} CC=${CC} CXX=${CXX} config=${CONFIG} all
