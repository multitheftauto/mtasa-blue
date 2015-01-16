#! /bin/sh

#
# To compile with symbols:
#   ./initial-install.sh -g
#

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

if [ $1 = "-g" ]; then
    ./configure CFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized' CXXFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized' CPPFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized' --with-pic --disable-system-pcre
else
    ./configure CFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized' CXXFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized' CPPFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized' --with-pic --disable-system-pcre
fi

# Remove previous build files #2
make clean
rm ./MTA10_Server/core/core.la
rm ./MTA10_Server/dbconmy/dbconmy.la
rm ./MTA10_Server/mods/deathmatch/deathmatch.la
rm ./Shared/XML/xmll.la
rm ./MTA10_Server/net/net.la

# Build breakpad
cd ./vendor/google-breakpad
./configure
make
cd ../..

# then you have makefiles and the source can be compiled :)
# building in parallel mode (use -j<JOBS>)
if [ $1 = "-g" ]; then
    make CFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized' CXXFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized' CPPFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized' >_make.log
else
    make CFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized' CXXFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized' CPPFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized' >_make.log
fi

