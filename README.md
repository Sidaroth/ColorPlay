ColorPlay
=========

Color play project for vitensenteret Gjøvik, through the Serious Games course at Gjøvik University College. 


This project requires:
- SFML (2.1): http://www.sfml-dev.org/download.php
- cURL: http://curl.haxx.se/download.html
- PSMoveAPI: https://thp.io/2010/psmove/#downloads


External Library locations (relative to the makefile) are generally:
- ..\lib\library\lib
- ..\lib\library\include

For more detail on include paths to external libraries consult the makefile.


To build the project, make sure all dependencies are installed (see above), then:
1. make
2. ./run.sh - requires sudo because of PSMove Camera calibration. 

run.sh adds the correct paths to the runtime linker. (This is needed unless the libraries are installed directly on the system)