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

# Build compatibility libraries
# 64-bit
if [ ! -d utils/compat/x64 ]; then
    mkdir utils/compat/x64
    objcopy --redefine-syms=utils/compat/glibc_version.redef /usr/lib/gcc/x86_64-linux-gnu/5/libstdc++.a utils/compat/x64/libstdc++.a
    objcopy --redefine-syms=utils/compat/glibc_version.redef /usr/lib/x86_64-linux-gnu/libmysqlclient.a utils/compat/x64/libmysqlclient.a
fi

# 32-bit
if [ ! -d utils/compat/x86 ]; then
    apt-get install -y libmysqlclient-dev:i386 # Install here as x64 and x86 cannot be installed in parallel
    mkdir utils/compat/x86
    objcopy --redefine-syms=utils/compat/glibc_version.redef /usr/lib/gcc/x86_64-linux-gnu/5/32/libstdc++.a utils/compat/x86/libstdc++.a
    objcopy --redefine-syms=utils/compat/glibc_version.redef /usr/lib/i386-linux-gnu/libmysqlclient.a utils/compat/x86/libmysqlclient.a
fi

# Start manual building
export GLIBC_COMPAT=true
./linux-build.sh $BUILD_BITS
