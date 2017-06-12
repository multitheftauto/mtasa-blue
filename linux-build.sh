#!/bin/bash

PREMAKE5=utils/premake5
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

# Build!
NUM_CORES=$(grep -c ^processor /proc/cpuinfo)
cd Build/
make -j$NUM_CORES config=$CONFIG all
