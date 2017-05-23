#!/bin/bash

if [[ "$(uname -m)" == 'x86_64' ]]; then
    PREMAKE5=utils/premake5_x64
    CONFIG=release_x64
else
    PREMAKE5=utils/premake5_x86
    CONFIG=release_x86
fi

# Clean old build files
rm -Rf Build/
rm -Rf Bin/

# Generate makefiles
$PREMAKE5 gmake

# Build!
cd Build/
make config=$CONFIG all
