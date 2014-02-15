#!/bin/bash
sudo LD_LIBRARY_PATH=../lib/poco/lib 	# Force the dynamic linker to check this path for poco.
sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib/SFML/lib
sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib/psmoveapi/build
echo "Adding library search path(s): $LD_LIBRARY_PATH"

./ColorPlay.exe
