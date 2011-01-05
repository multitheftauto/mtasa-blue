#! /bin/sh

# generate autotools build system files
autoreconf -fiv

# then look at the configure options:
#   ./configure --help

# on a 64bit system, while there's no 64bit net.so
# and probably some unknown issues you should compile and link
# 32bit objects like so:
export PKG_CONFIG_PATH=/usr/lib32/pkgconfig
./configure LDFLAGS="-m32" CPPFLAGS="-m32" CFLAGS="-m32" CXXFLAGS="-m32"

# then you have makefiles and the source can be compiled :)
# building in parallel mode (use -j<JOBS>)
make -j2

# then install mta
./mta-install.sh
