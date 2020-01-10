#!/bin/bash

# Find premake binary location
if [ "$(uname)" == "Darwin" ]; then
    PREMAKE5=utils/premake5-macos
else
    PREMAKE5=utils/premake5
fi

# Install data files
$PREMAKE5 install_data

# Optionally install resources
read -p "Install resources? [y/N] " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    $PREMAKE5 install_resources
fi
