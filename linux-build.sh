#!/bin/bash

# Find premake binary location
if [ "$(uname)" == "Darwin" ]; then
    PREMAKE5=utils/premake5-macos
else
    PREMAKE5=utils/premake5
fi

ENV_CONFIG=$CONFIG
DEFAULT=2

# Debug vs Release
if [[ $2 = "release" ]] || [[ $2 = "debug" ]]; then
    CONFIG=$2
elif [[ $2 != "" ]]; then
    printf "Unknown build type \"$2\" provided\n"
    exit 1
else
    CONFIG=release
    ((DEFAULT--))
fi

# 32bit vs 64bit
if [[ $1 = "32" ]] || [[ $1 = "x86" ]]; then
    CONFIG=${CONFIG}_x86
elif [[ $1 = "64" ]] || [[ $1 = "x64" ]]; then
    CONFIG=${CONFIG}_x64
elif [[ $1 != "" ]]; then
    printf "Unknown architecture \"$1\" provided\n"
    exit 1
else
    CONFIG=${CONFIG}_x64
    ((DEFAULT--))
fi

# Only apply $CONFIG from environment if no args provided
if [[ $ENV_CONFIG != "" ]]; then
    if [[ $DEFAULT != 0 ]]; then
        printf "Ignoring provided \$CONFIG environment variable, "
    else
        CONFIG=$ENV_CONFIG
    fi
fi

printf "\$CONFIG=$CONFIG\n"

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
make -C Build/ -j$NUM_CORES config=$CONFIG all
