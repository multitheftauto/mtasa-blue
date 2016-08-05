#!/bin/bash

# Clean old build files
rm -Rf Build/
rm -Rf Bin/

# Generate makefiles
utils/premake5_x64 gmake

# Build!
cd Build/
make config=release_x86 all
