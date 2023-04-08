#!/bin/bash -e

# If configured as a build agent, we start the TeamCity agent
if [[ $AS_BUILDAGENT = "1" ]]; then
    # https://github.com/JetBrains/teamcity-docker-minimal-agent/blob/master/Dockerfile#L17
    exec /run-services.sh
fi

# Manually invoke build process
umask 000
if [ ! -f ./premake5.lua ]; then
    git clone --depth=1 https://github.com/multitheftauto/mtasa-blue.git .
fi

# Start manual building
./linux-build.sh --arch=$BUILD_ARCHITECTURE --config=$BUILD_CONFIG
