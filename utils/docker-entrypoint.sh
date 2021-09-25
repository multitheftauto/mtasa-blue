#!/bin/bash

# 64-bits
if [ ! -d /compat/x64 ]; then
    mkdir -p /compat/x64
    objcopy --redefine-syms=/compat/glibc_version_x64.redef "$(gcc --print-file-name=libstdc++.a)" /compat/x64/libstdc++.a
    objcopy --redefine-syms=/compat/glibc_version_x64.redef /usr/lib/x86_64-linux-gnu/libmysqlclient.a /compat/x64/libmysqlclient.a
fi

# 32-bits
if [ ! -d /compat/x86 ] && [[ $BUILD_BITS = "32" ]]; then
    apt-get update && apt-get install -y libmysqlclient-dev:i386
    mkdir -p /compat/x86
    objcopy --redefine-syms=/compat/glibc_version_x86.redef "$(gcc -m32 --print-file-name=libstdc++.a)" /compat/x86/libstdc++.a
    objcopy --redefine-syms=/compat/glibc_version_x86.redef /usr/lib/i386-linux-gnu/libmysqlclient.a /compat/x86/libmysqlclient.a
fi


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

# Start manual building
export GLIBC_COMPAT=true

# Set the default build target
if [ -z "$BUILD_TARGET" ]
then
      BUILD_TARGET=release
fi
echo $BUILD_TARGET

./linux-build.sh $BUILD_BITS $BUILD_TARGET
