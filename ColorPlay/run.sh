#!/bin/bash
export LD_LIBRARY_PATH=../lib/poco/lib 	# Force the dynamic linker to check this path for poco.
echo "Adding poco to library search path: $LD_LIBRARY_PATH"

./ColorPlay.exe
