FROM ghcr.io/multitheftauto/mtasa-blue-gcc:15-noble

ARG GLIBC_COMPAT=true

# Use a combined APT sources list for amd64, i386, armhf and arm64.
COPY ubuntu.sources  /etc/apt/sources.list.d/ubuntu.sources

# 1. Add the supported architectures.
# 2. Install all compilers and build-time dependencies (libncurses, libmysqlclient) for every architecture.
# 3. Manually download, modify, and install libmysqlclient-dev, because it can't be installed in parallel.
# 4. Remove cache and temporary files.
RUN set -ex ;\
    dpkg --add-architecture i386  ;\
    dpkg --add-architecture armhf ;\
    dpkg --add-architecture arm64 ;\
    apt-get update ;\
    DEBIAN_FRONTEND=noninteractive apt-get install -y make binutils \
        libncurses-dev:arm64 libmysqlclient21:arm64 zlib1g-dev:arm64 libzstd-dev:arm64 libssl-dev:arm64 \
        libncurses-dev:armhf libmysqlclient21:armhf zlib1g-dev:armhf libzstd-dev:armhf libssl-dev:armhf \
        libncurses-dev:i386  libmysqlclient21:i386  zlib1g-dev:i386  libzstd-dev:i386  libssl-dev:i386  \
        libncurses-dev       libmysqlclient21       zlib1g-dev       libzstd-dev       libssl-dev       \
    ;\
    apt-get upgrade -y ;\
    cd /tmp ;\
    apt-get download \
        libmysqlclient-dev:armhf \
        libmysqlclient-dev:arm64 \
        libmysqlclient-dev:i386  \
        libmysqlclient-dev       \
    ;\
    dpkg-deb -R libmysqlclient-dev_*_armhf.deb libmysqlclient-dev-armhf ;\
    dpkg-deb -R libmysqlclient-dev_*_arm64.deb libmysqlclient-dev-arm64 ;\
    dpkg-deb -R libmysqlclient-dev_*_i386.deb  libmysqlclient-dev-i386  ;\
    dpkg-deb -R libmysqlclient-dev_*_amd64.deb libmysqlclient-dev-amd64 ;\
    echo 'Multi-Arch: same' | tee -a \
        libmysqlclient-dev-amd64/DEBIAN/control \
        libmysqlclient-dev-i386/DEBIAN/control  \
        libmysqlclient-dev-armhf/DEBIAN/control \
        libmysqlclient-dev-arm64/DEBIAN/control \
    ;\
    dpkg-deb -b libmysqlclient-dev-amd64 libmysqlclient-dev-amd64.deb ;\
    dpkg-deb -b libmysqlclient-dev-i386  libmysqlclient-dev-i386.deb  ;\
    dpkg-deb -b libmysqlclient-dev-armhf libmysqlclient-dev-armhf.deb ;\
    dpkg-deb -b libmysqlclient-dev-arm64 libmysqlclient-dev-arm64.deb ;\
    dpkg -i --force-overwrite \
        libmysqlclient-dev-armhf.deb \
        libmysqlclient-dev-arm64.deb \
        libmysqlclient-dev-i386.deb  \
        libmysqlclient-dev-amd64.deb \
    ;\
    rm -rf /tmp/* /var/lib/apt/lists/*

# Downgrade glibc symbols for backwards compatibility.
COPY compat /compat

RUN set -ex ;\
    mkdir -p /compat/x64 /compat/x86 ;\
    objcopy --redefine-syms=/compat/glibc_version_x64.redef "$(gcc      --print-file-name=libstdc++.a)"   /compat/x64/libstdc++.a ;\
    objcopy --redefine-syms=/compat/glibc_version_x86.redef "$(gcc -m32 --print-file-name=libstdc++.a)"   /compat/x86/libstdc++.a 

VOLUME  /build
WORKDIR /build

COPY docker-entrypoint.sh  /docker-entrypoint.sh

RUN chmod +x /docker-entrypoint.sh

ENTRYPOINT ["/bin/bash", "/docker-entrypoint.sh"]
