#! /bin/sh

#
# To compile with symbols:
#   ./mta-build.sh -g
#

# Remove previous build files
make clean

# generate autotools build system files
autoreconf -fiv

# then look at the configure options:
#   ./configure --help
if [ "$1" = "-g" ]; then
    ./configure --with-pic --disable-system-pcre --enable-utf8 \
                  CFLAGS='-g -O2 -fPIC -DPIC' \
                CXXFLAGS='-g -O2 -fPIC -DPIC -std=c++0x' \
                CPPFLAGS='-g -O2 -fPIC -DPIC'
else
    ./configure --with-pic --disable-system-pcre --enable-utf8 \
                  CFLAGS='-O2 -fPIC -DPIC' \
                CXXFLAGS='-O2 -fPIC -DPIC -std=c++0x' \
                CPPFLAGS='-O2 -fPIC -DPIC'
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
make CXXFLAGS='-Wno-sign-compare'
cd ../..

# Build Crypto++
cd ./vendor/cryptopp
if [ "$1" = "-g" ]; then
	make static WITH_SYMBOLS=1
else
	make static
fi
cd ../..

# then you have makefiles and the source can be compiled :)
# building in parallel mode (use -j<JOBS>)
if [ "$1" = "-g" ]; then
    make \
                  CFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized -Wno-narrowing' \
                CXXFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized -Wno-narrowing -std=c++0x' \
                CPPFLAGS='-g -O2 -fPIC -DPIC -Wno-uninitialized -Wno-narrowing' >_make.log
else
    make \
                  CFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized -Wno-narrowing' \
                CXXFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized -Wno-narrowing -std=c++0x' \
                CPPFLAGS='-O2 -fPIC -DPIC -Wno-uninitialized -Wno-narrowing' >_make.log
fi

