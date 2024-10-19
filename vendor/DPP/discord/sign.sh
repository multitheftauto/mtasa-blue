#!/bin/sh
rm -rf build/sign
mkdir -p build/sign
cd build/sign
gh release download "$1" -A tar.gz
sleep 2
gh release download "$1" -A zip
sleep 2
gh release download "$1"
sleep 2
rm -fv *.asc
find . -type f -exec gpg --armor --detach-sign {} \;
gh release upload "$1" ./* --clobber

