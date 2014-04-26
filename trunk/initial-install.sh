#! /bin/sh

# Remove previous build files
make clean

# generate autotools build system files
autoreconf -fiv

# then look at the configure options:
#   ./configure --help

# on a 64bit system, while there's no 64bit net.so
# and probably some unknown issues you should compile and link
# 32bit objects like so:
#export PKG_CONFIG_PATH=/usr/lib32/pkgconfig
#./configure LDFLAGS="-m32" CPPFLAGS="-m32" CFLAGS="-m32" CXXFLAGS="-m32" --enable-silent-rules $@

./configure CFLAGS='-O3 -fPIC -DPIC' CXXFLAGS='-O3 -fPIC -DPIC' CPPFLAGS='-O3 -fPIC -DPIC' --with-pic --disable-system-pcre

# Remove previous build files #2
make clean
rm ./MTA10_Server/core/core.la
rm ./MTA10_Server/dbconmy/dbconmy.la
rm ./MTA10_Server/mods/deathmatch/deathmatch.la
rm ./Shared/XML/xmll.la
rm ./MTA10_Server/net/net.la

# then you have makefiles and the source can be compiled :)
# building in parallel mode (use -j<JOBS>)
make CFLAGS='-O3 -fPIC -DPIC' CXXFLAGS='-O3 -fPIC -DPIC' CPPFLAGS='-O3 -fPIC -DPIC' >_make.log
