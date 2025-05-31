FROM ubuntu:focal

ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    LANGUAGE=C.UTF-8 \
    GLIBC_COMPAT=true

# Trust the signing key for "PPA for Ubuntu Toolchain Uploads".
# Source: https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x2c277a0a352154e5
COPY ubuntu-toolchain-r-ubuntu-test.asc  /etc/apt/trusted.gpg.d/

# Use a combined APT sources list for amd64, i386, armhf, arm64, and PPA for Ubuntu Toolchain Uploads.
COPY sources.list  /etc/apt/sources.list

# 1. Add the supported architectures.
# 2. Install all compilers and build-time dependencies (libncurses, libmysqlclient) for every architecture.
# 3. Manually download, modify, and install libmysqlclient-dev, because it can't be installed in parallel.
# 4. Remove cache and temporary files.
RUN    dpkg --add-architecture i386  \
    && dpkg --add-architecture armhf \
    && dpkg --add-architecture arm64 \
    && apt-get update \
    && apt-get install -y --no-install-recommends make  \
        libncurses-dev:arm64 libmysqlclient21:arm64 zlib1g-dev:arm64 libssl-dev:arm64 g++-10-arm-linux-gnueabihf \
        libncurses-dev:armhf libmysqlclient21:armhf zlib1g-dev:armhf libssl-dev:armhf g++-10-aarch64-linux-gnu   \
        libncurses-dev:i386  libmysqlclient21:i386  zlib1g-dev:i386  libssl-dev:i386  g++-10-multilib            \
        libncurses-dev       libmysqlclient21       zlib1g-dev       libssl-dev                                  \
    && apt-get upgrade -y \
    && cd /tmp \
    && apt-get download \
        libmysqlclient-dev:armhf \
        libmysqlclient-dev:arm64 \
        libmysqlclient-dev:i386  \
        libmysqlclient-dev       \
    && dpkg-deb -R libmysqlclient-dev_*_armhf.deb libmysqlclient-dev-armhf \
    && dpkg-deb -R libmysqlclient-dev_*_arm64.deb libmysqlclient-dev-arm64 \
    && dpkg-deb -R libmysqlclient-dev_*_i386.deb  libmysqlclient-dev-i386  \
    && dpkg-deb -R libmysqlclient-dev_*_amd64.deb libmysqlclient-dev-amd64 \
    && echo 'Multi-Arch: same' | tee -a \
        libmysqlclient-dev-amd64/DEBIAN/control \
        libmysqlclient-dev-i386/DEBIAN/control  \
        libmysqlclient-dev-armhf/DEBIAN/control \
        libmysqlclient-dev-arm64/DEBIAN/control \
    && dpkg-deb -b libmysqlclient-dev-amd64 libmysqlclient-dev-amd64.deb \
    && dpkg-deb -b libmysqlclient-dev-i386  libmysqlclient-dev-i386.deb  \
    && dpkg-deb -b libmysqlclient-dev-armhf libmysqlclient-dev-armhf.deb \
    && dpkg-deb -b libmysqlclient-dev-arm64 libmysqlclient-dev-arm64.deb \
    && dpkg -i --force-overwrite \
        libmysqlclient-dev-armhf.deb \
        libmysqlclient-dev-arm64.deb \
        libmysqlclient-dev-i386.deb  \
        libmysqlclient-dev-amd64.deb \
    && rm -rf /tmp/* /var/lib/apt/lists/*

# Downgrade glibc symbols for backwards compatibility.
COPY compat /compat

RUN mkdir -p /compat/x64 /compat/x86 \
    && objcopy --redefine-syms=/compat/glibc_version_x64.redef "$(gcc-10 --print-file-name=libstdc++.a)"      /compat/x64/libstdc++.a      \
    && objcopy --redefine-syms=/compat/glibc_version_x64.redef /usr/lib/x86_64-linux-gnu/libmysqlclient.a     /compat/x64/libmysqlclient.a \
    && objcopy --redefine-syms=/compat/glibc_version_x86.redef "$(gcc-10 -m32 --print-file-name=libstdc++.a)" /compat/x86/libstdc++.a      \
    && objcopy --redefine-syms=/compat/glibc_version_x86.redef /usr/lib/i386-linux-gnu/libmysqlclient.a       /compat/x86/libmysqlclient.a

VOLUME  /build
WORKDIR /build

COPY docker-entrypoint.sh  /docker-entrypoint.sh

RUN chmod +x /docker-entrypoint.sh

ENTRYPOINT ["/bin/bash", "/docker-entrypoint.sh"]
