#! /bin/sh
#
# First, compile pcre
#
cd vendor/pcre
autoreconf -fiv
./configure --disable-shared
make libpcre.la
cd ../..

#
# Then, compile mta
#
autoreconf -fiv
./configure
./mta-install.sh
