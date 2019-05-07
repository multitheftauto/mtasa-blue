#!/usr/bin/env bash 
CONFIGURATION=$1
C_COMPILER=$2
CXX_COMPILER=$3

if [[ ! -z "$2" ]] && [[ -z "$3" ]]
then
	echo "You must specify both a C and a C++ compiler, or leave both unspecified."
fi

if [[ -z "$C_COMPILER" ]]
then
	C_COMPILER="clang"
	CXX_COMPILER="clang++"
	ADDITIONAL_FLAGS="-D_CMAKE_TOOLCHAIN_PREFIX=llvm-"
fi

BUILD_DIR="build"
TARGET_FOLDER_NAME="${CONFIGURATION}-${ARCHITECTURE}-${C_COMPILER}"

if [[ ! -z "$BUILD_FOLDER_SUFFIX" ]]
then
	TARGET_FOLDER_NAME="${TARGET_FOLDER_NAME}-${BUILD_FOLDER_SUFFIX}"
fi

TARGET_DIR="$BUILD_DIR/$TARGET_FOLDER_NAME"

echo "Building into $TARGET_DIR"

mkdir --parents $TARGET_DIR
cd $TARGET_DIR

export CC=$C_COMPILER
export CXX=$CXX_COMPILER

cmake -DCMAKE_BUILD_TYPE=$CONFIGURATION $ADDITIONAL_FLAGS $@ $OLDPWD -G Ninja

pushd ../
	# For simplicity of subsequent scripts, create a symlink to the last created build
	rm -f current
	ln -s $TARGET_FOLDER_NAME current
popd
