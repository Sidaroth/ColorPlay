# The compiler
CC=g++
# Flags for the compiler
CFLAGS=-c -std=gnu++11 -Wall

# Name of the executable
EXECUTABLE=ColorPlay.exe
# Output files from the executable
OUTPUT=test.csv
# Source files
SOURCES=main.cpp LogModule.cpp StringQueue.cpp BulbHandler.cpp MoveHandler.cpp WindowHandler.cpp BulbMath.cpp
# The objects
OBJECTS=$(SOURCES:.cpp=.o)

# EDIT these to add additional libraries. 
# Libraries to include. (Prefix all additional libs with -L, like such: -L/example/dir/lib)
LIB=-L../lib/poco/lib -L../lib/SFML/lib -L../lib/psmoveapi/build
# Library files to include (Prefix all addtional include dirs with -I, like such: -I/example/dir/include)
INC=-I../lib/poco/include -I../lib/SFML/include -I../lib/psmoveapi/include
# Flags for the linker. (When adding new libraries, add another flag here, as such: -lexample-lib) PS: These are the libexample-lib.so files, with the lib prefix and .so extension removed. 
POCOFLAGS=   -lPocoFoundation -lPocoNet
SFMLFLAGS=   -lsfml-graphics -lsfml-window -lsfml-system
PSMOVEFLAGS= -lpsmoveapi -lpsmoveapi_tracker -lopencv_highgui -lopencv_core

LFLAGS=-lpthread $(POCOFLAGS) $(SFMLFLAGS) $(PSMOVEFLAGS)

### !!!! DO NOT TOUCH ANYTHING BELOW THIS LINE EXCEPT CLEAN!!!! ###

build: $(SOURCES) $(EXECUTABLE)

all: 
	build

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIB) $(LFLAGS) -o $@

.cpp.o:
	$(CC) $(INC) $(CFLAGS) $< -o $@


# Run: make clean
# if you need to update object files / the executable and the make file does not want to update. This can sometimes happen if only header files have been edited. 
# Edit this (read: add to this) if you need to remove additional files when running "make clean". 
clean:
	rm -f *.o
	rm -f $(EXECUTABLE)
	rm -f $(OUTPUT)
