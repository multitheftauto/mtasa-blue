#!/bin/bash

# 64-bits
if [ ! -d /compat/x64 ] && [ -f /usr/lib/x86_64-linux-gnu/libmysqlclient.a ]; then
    mkdir -p /compat/x64
    objcopy --redefine-syms=/compat/glibc_version_x64.redef "$(gcc --print-file-name=libstdc++.a)" /compat/x64/libstdc++.a
    objcopy --redefine-syms=/compat/glibc_version_x64.redef /usr/lib/x86_64-linux-gnu/libmysqlclient.a /compat/x64/libmysqlclient.a
fi

# 32-bits
if [ ! -d /compat/x86 ] && [ -f /usr/lib/i386-linux-gnu/libmysqlclient.a ]; then
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

./linux-build.sh --arch=$BUILD_ARCHITECTURE --config=$BUILD_CONFIG
