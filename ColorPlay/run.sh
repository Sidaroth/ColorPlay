#!/bin/bash
export LD_LIBRARY_PATH=../lib/poco/lib 	# Force the dynamic linker to check this path for poco.
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib/SFML/lib
echo "Adding library search path(s): $LD_LIBRARY_PATH"

./ColorPlay.exe
