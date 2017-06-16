#!/bin/bash

PREMAKE5=utils/premake5
if [ "$1" -eq "32" ]; then
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
cd Build/
make config=$CONFIG all
