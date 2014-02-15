#!/bin/bash
export LD_LIBRARY_PATH=../lib/curl/lib 	# Force the dynamic linker to check this path for curl libs.
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib/SFML/lib 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib/psmoveapi/build
echo "Adding library search path(s): $LD_LIBRARY_PATH"

./ColorPlay.exe
