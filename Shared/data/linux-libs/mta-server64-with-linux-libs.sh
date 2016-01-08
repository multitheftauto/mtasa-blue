#!/bin/bash

#
# mta-server-with-linux-libs.sh
# mta-server64-with-linux-libs.sh
#
# Launch mta-server after adding 'linux-libs' to the library search path
#

if [[ "$0" != *"64"* ]]; then
    # 32 bit
    LAUNCHER="mta-server"
    LINUXLIBS="x86/linux-libs"
else
    # 64 bit
    LAUNCHER="mta-server64"
    LINUXLIBS="x64/linux-libs"
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LIBDIR=$DIR"/"$LINUXLIBS

if [[ "$LD_LIBRARY_PATH" == "" ]]; then
    # LD_LIBRARY_PATH is empty
    export LD_LIBRARY_PATH=$LIBDIR
elif [[ "$LD_LIBRARY_PATH" != *"$LIBDIR"* ]]; then
    # LD_LIBRARY_PATH does not contain LIBDIR
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH";"$LIBDIR
fi

# Pass command line arguments to launcher
CMD="$DIR/$LAUNCHER $*"
$CMD
exit $?
