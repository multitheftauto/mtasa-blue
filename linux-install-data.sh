#!/bin/bash

if [[ "$(uname -m)" == 'x86_64' ]]; then
    PREMAKE5=utils/premake5_x64
else
    PREMAKE5=utils/premake5_x86
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
