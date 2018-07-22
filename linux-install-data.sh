#!/bin/bash

PREMAKE5=utils/premake5

# Install data files
$PREMAKE5 install_data

# Optionally install resources
read -p "Install resources? [y/N] " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    $PREMAKE5 install_resources
fi
