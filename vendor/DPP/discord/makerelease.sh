#!/bin/bash
NEWVER=$(grep DPP_VERSION_TEXT include/dpp/version.h | sed 's/\|/ /' | awk '{print $4}')
echo "Building and tagging release $NEWVER"
mkdir temp
cd temp
echo "Download assets from CI..."
gh run list -w "D++ CI" | grep $'\t'master$'\t' | grep ^completed | head -n1
gh run download $(gh run list -w "D++ CI" | grep $'\t'master$'\t' | grep ^completed | head -n1 | awk '{ printf $(NF-2) }')
echo "Move assets..."
mkdir assets

# rpm
mv "./libdpp - RPM Package amd64/libdpp-$NEWVER-Linux.rpm" "./assets/libdpp-$NEWVER-linux-x64.rpm"
mv "./libdpp - RPM Package Linux x86/libdpp-$NEWVER-Linux.rpm" "./assets/libdpp-$NEWVER-linux-i386.rpm"
mv "./libdpp - RPM Package arm64/libdpp-$NEWVER-Linux.rpm" "./assets/libdpp-$NEWVER-linux-rpi-arm64.rpm"
mv "./libdpp - RPM Package arm7hf/libdpp-$NEWVER-Linux.rpm" "./assets/libdpp-$NEWVER-linux-rpi-arm7hf.rpm"
mv "./libdpp - RPM Package ARMv6/libdpp-$NEWVER-Linux.rpm" "./assets/libdpp-$NEWVER-linux-rpi-arm6.rpm"

# deb
mv "./libdpp - Debian Package amd64/libdpp-$NEWVER-Linux.deb" "./assets/libdpp-$NEWVER-linux-x64.deb"
mv "./libdpp - Debian Package Linux x86/libdpp-$NEWVER-Linux.deb" "./assets/libdpp-$NEWVER-linux-i386.deb"
mv "./libdpp - Debian Package arm64/libdpp-$NEWVER-Linux.deb" "./assets/libdpp-$NEWVER-linux-rpi-arm64.deb"
mv "./libdpp - Debian Package arm7hf/libdpp-$NEWVER-Linux.deb" "./assets/libdpp-$NEWVER-linux-rpi-arm7hf.deb"
mv "./libdpp - Debian Package ARMv6/libdpp-$NEWVER-Linux.deb" "./assets/libdpp-$NEWVER-linux-rpi-arm6.deb"

# win vs2019
mv "./libdpp - Windows x64-Release-vs2019/libdpp-$NEWVER-win64.zip" "./assets/libdpp-$NEWVER-win64-release-vs2019.zip"
mv "./libdpp - Windows x64-Debug-vs2019/libdpp-$NEWVER-win64.zip" "./assets/libdpp-$NEWVER-win64-debug-vs2019.zip"
mv "./libdpp - Windows x86-Release-vs2019/libdpp-$NEWVER-win32.zip" "./assets/libdpp-$NEWVER-win32-release-vs2019.zip"
mv "./libdpp - Windows x86-Debug-vs2019/libdpp-$NEWVER-win32.zip" "./assets/libdpp-$NEWVER-win32-debug-vs2019.zip"

# win vs2022
mv "./libdpp - Windows x64-Release-vs2022/libdpp-$NEWVER-win64.zip" "./assets/libdpp-$NEWVER-win64-release-vs2022.zip"
mv "./libdpp - Windows x64-Debug-vs2022/libdpp-$NEWVER-win64.zip" "./assets/libdpp-$NEWVER-win64-debug-vs2022.zip"
mv "./libdpp - Windows x86-Release-vs2022/libdpp-$NEWVER-win32.zip" "./assets/libdpp-$NEWVER-win32-release-vs2022.zip"
mv "./libdpp - Windows x86-Debug-vs2022/libdpp-$NEWVER-win32.zip" "./assets/libdpp-$NEWVER-win32-debug-vs2022.zip"

cd assets

## VS2019

# 64 bit windows
mkdir -p "libdpp-$NEWVER-win64/bin"
cp ../../win32/bin/*.dll "libdpp-$NEWVER-win64/bin"
zip -g "libdpp-$NEWVER-win64-release-vs2019.zip" "libdpp-$NEWVER-win64/bin/"*
rm -rf "libdpp-$NEWVER-win64"

mkdir -p "libdpp-$NEWVER-win64/bin"
cp ../../win32/bin/*.dll "libdpp-$NEWVER-win64/bin"
zip -g "libdpp-$NEWVER-win64-debug-vs2019.zip" "libdpp-$NEWVER-win64/bin/"*
rm -rf "libdpp-$NEWVER-win64"

# 32 bit windows
mkdir -p "libdpp-$NEWVER-win32/bin"
cp ../../win32/32/bin/*.dll "libdpp-$NEWVER-win32/bin"
zip -g "libdpp-$NEWVER-win32-release-vs2019.zip" "libdpp-$NEWVER-win32/bin/"*
rm -rf "libdpp-$NEWVER-win32"

mkdir -p "libdpp-$NEWVER-win32/bin"
cp ../../win32/32/bin/*.dll "libdpp-$NEWVER-win32/bin"
zip -g "libdpp-$NEWVER-win32-debug-vs2019.zip" "libdpp-$NEWVER-win32/bin/"*
rm -rf "libdpp-$NEWVER-win32"

## VS2022

# 64 bit windows
mkdir -p "libdpp-$NEWVER-win64/bin"
cp ../../win32/bin/*.dll "libdpp-$NEWVER-win64/bin"
zip -g "libdpp-$NEWVER-win64-release-vs2022.zip" "libdpp-$NEWVER-win64/bin/"*
rm -rf "libdpp-$NEWVER-win64"

mkdir -p "libdpp-$NEWVER-win64/bin"
cp ../../win32/bin/*.dll "libdpp-$NEWVER-win64/bin"
zip -g "libdpp-$NEWVER-win64-debug-vs2022.zip" "libdpp-$NEWVER-win64/bin/"*
rm -rf "libdpp-$NEWVER-win64"

# 32 bit windows
mkdir -p "libdpp-$NEWVER-win32/bin"
cp ../../win32/32/bin/*.dll "libdpp-$NEWVER-win32/bin"
zip -g "libdpp-$NEWVER-win32-release-vs2022.zip" "libdpp-$NEWVER-win32/bin/"*
rm -rf "libdpp-$NEWVER-win32"

mkdir -p "libdpp-$NEWVER-win32/bin"
cp ../../win32/32/bin/*.dll "libdpp-$NEWVER-win32/bin"
zip -g "libdpp-$NEWVER-win32-debug-vs2022.zip" "libdpp-$NEWVER-win32/bin/"*
rm -rf "libdpp-$NEWVER-win32"


cd ..
echo "Create release..."
gh release create "v$NEWVER" --draft --title "v$NEWVER release" --notes "$(/usr/bin/php -d error_reporting=off ../buildtools/changelog.php)" ./assets/*.zip ./assets/*.deb ./assets/*.rpm
gh release create -R brainboxdotcc/windows-bot-template "v$NEWVER" --draft --title "v$NEWVER release" --notes "This release packages DPP v$NEWVER into the windows bot template"
echo "Cleaning up..."
cd ..
rm -rf temp

./sign.sh v$NEWVER

