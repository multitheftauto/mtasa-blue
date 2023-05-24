#!/bin/bash

# If configurede as a build agent, start Teamcity agent
if [[ $AS_BUILDAGENT = "1" ]]; then
    # https://github.com/JetBrains/teamcity-docker-minimal-agent/blob/master/Dockerfile#L17
    exec /run-services.sh
fi

# Manually invoke build process
# So, first make a shallow clone of the repository if it not exists
umask 000
if [ ! -f ./premake5.lua ]; then
    git clone --depth=1 https://github.com/multitheftauto/mtasa-blue.git .
fi

./linux-build.sh --os=macosx --arch=$BUILD_ARCHITECTURE --config=$BUILD_CONFIG
