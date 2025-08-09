#!/bin/bash

if [ -z "$MSBUILDPATH" ]; then
    echo "Could not find MSBuild. Make sure the MSBUILDPATH environment variable points"
    echo "to either MSBuild.exe or a script that handles the invocation of MSBuild.exe."
    echo ""
    echo "If using msvc-wine, it should point to /path/to/msvc-wine-install-dir/bin/x64/msbuild"
    exit 1
fi
echo "Found MSBuild at: $MSBUILDPATH"

BUILD_CONFIGURATION=Release
BUILD_PLATFORM=Win32

# Read configuration (1st parameter)
case ${1,,} in
    debug)   BUILD_CONFIGURATION=Debug ;;
    release) BUILD_CONFIGURATION=Release ;;
    "") ;;
    *) echo "Invalid first argument $1. Using default configuration $BUILD_CONFIGURATION." ;;
esac

# Read platform (2nd parameter)
case ${2,,} in
    win32) BUILD_PLATFORM=Win32 ;;
    x64)   BUILD_PLATFORM=x64 ;;
    arm)   BUILD_PLATFORM=ARM ;;
    arm64) BUILD_PLATFORM=ARM64 ;;
    "") ;;
    *) echo "Invalid second argument $2. Using default platform $BUILD_PLATFORM." ;;
esac

echo "Build configuration:"
echo "  BUILD_CONFIGURATION = $BUILD_CONFIGURATION"
echo "  BUILD_PLATFORM = $BUILD_PLATFORM"

# Create solution (ignoring pause)
wine win-create-projects.bat < /dev/null
echo ""

# Start compiling
"$MSBUILDPATH" Build/MTASA.sln -property:Configuration="$BUILD_CONFIGURATION" -property:Platform="$BUILD_PLATFORM" -maxCpuCount