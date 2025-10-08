#!/bin/bash

: ${arch_value:=x64}

for arg in "$@"; do
    case $arg in
        --arch=*)
            arch_value="${arg#--arch=}"
            break
        ;;
    esac
done

case $arch_value in
    32|x86)
        prefix=i386-linux-gnu-
        triplet=x86_64-linux-gnu-
    ;;
    64|x64)
        prefix=x86_64-linux-gnu-
        triplet=$prefix
    ;;
    arm)
        prefix=arm-linux-gnueabihf-
        triplet=$prefix
    ;;
    arm64)
        prefix=aarch64-linux-gnu-
        triplet=$prefix
    ;;
    *)
        echo "Error: Invalid build architecture" >&2
        exit 1
esac

: ${GCC_PREFIX:=$prefix}
: ${AR:=${triplet}gcc-ar-15}
: ${CC:=${triplet}gcc-15}
: ${CXX:=${triplet}g++-15}

export GCC_PREFIX AR CC CXX

if [ -f ./linux-build.sh ]; then
    chmod +x ./linux-build.sh
    ./linux-build.sh "$@"
fi
