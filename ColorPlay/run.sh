#!/bin/bash
echo "Adding library search path(s)"
sudo LD_LIBRARY_PATH=../lib/SFML/lib:../lib/psmoveapi/build:../lib/curl/lib ./ColorPlay.exe # Force the dynamic linker to check these paths for libs.
