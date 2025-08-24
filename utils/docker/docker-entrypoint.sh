#!/bin/bash

if [ -f ./linux-build.sh ]; then
    chmod +x ./linux-build.sh
    ./linux-build.sh "$@"
fi
