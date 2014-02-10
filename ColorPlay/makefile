# The compiler
CC=g++
# Flags for the compiler
CFLAGS=-c -std=gnu++11 -Wall
# Flags for the linker. 
LFLAGS=-lpthread -lPocoFoundation -lPocoNet -lsfml-graphics -lsfml-window -lsfml-system -lpsmoveapi -lpsmoveapi_tracker
# Name of the executable
EXECUTABLE=ColorPlay.exe
# Output files from the executable
OUTPUT=test.csv
# Source files
SOURCES=main.cpp LogModule.cpp StringQueue.cpp BulbHandler.cpp MoveHandler.cpp WindowHandler.cpp
# The objects
OBJECTS=$(SOURCES:.cpp=.o)
# Libraries to include
LIB=-L../lib/poco/lib -L../lib/SFML/lib -L../lib/psmoveapi/build
# Files to include (from libraries)
INC=-I../lib/poco/include -I../lib/SFML/include -I../lib/psmoveapi/include


build: $(SOURCES) $(EXECUTABLE)

all: 
	build

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIB) $(LFLAGS) -o $@

.cpp.o:
	$(CC) $(INC) $(CFLAGS) $< -o $@

# Run: make clean
# if you need to update object files / the executable and the make file does not want to update. This can sometimes happen if only header files have been edited. 
clean:
	rm -f *.o
	rm -f $(EXECUTABLE)
	rm -f $(OUTPUT)