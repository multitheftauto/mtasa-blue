#!/bin/bash

# Find premake binary location
if [ "$(uname)" == "Darwin" ]; then
    PREMAKE5=utils/premake5-macos
else
    PREMAKE5=utils/premake5
fi

# 32bit vs 64bit
if [[ $1 = "32" ]]; then
    CONFIG=release_x86
else
    CONFIG=release_x64
fi

# Clean old build files
rm -Rf Build/
rm -Rf Bin/

# Generate makefiles
$PREMAKE5 gmake

# Number of cores
if [ "$(uname)" == "Darwin" ]; then
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    NUM_CORES=$(grep -c ^processor /proc/cpuinfo)
fi

# Build!
cd Build/
make -j$NUM_CORES config=$CONFIG all
