#!/usr/bin/env bash 
CORE_PATH=$1

if [ -f "$CORE_PATH" ]; then
	echo "Core found at $CORE_PATH." > $OUTPUT_TERM
	gdb -ex="bt" -ex="q" build/current/rectpack2D $CORE_PATH
else
	echo "Core not found at $CORE_PATH."
fi
