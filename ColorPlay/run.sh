#!/bin/bash
sudo LD_LIBRARY_PATH=../lib/SFML/lib:../lib/psmoveapi/build:../lib/curl/lib ./ColorPlay.exe # Force the dynamic linker to check these paths for libs.

#sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib/SFML/lib
#sudo LD_LIBRARY_PATH:=$LD_LIBRARY_PATH:../lib/psmoveapi/build
echo "Adding library search path(s): $LD_LIBRARY_PATH"