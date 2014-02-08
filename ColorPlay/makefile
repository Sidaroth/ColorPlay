# The compiler
CC=g++
# Flags for the compiler
CFLAGS=-c -std=gnu++11 -pthread -Wall
# Flags for the linker. 
LFLAGS=-pthread
# Name of the executable
EXECUTABLE=ColorPlay
# Source files
SOURCES=main.cpp LogModule.cpp StringQueue.cpp BulbHandler.cpp MoveHandler.cpp
# The objects
OBJECTS=$(SOURCES:.cpp=.o)
LIB=-L ../lib/poco/lib
INC=-I ../lib/poco/include

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LIB) $(LFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(INC) $(CFLAGS) $< -o $@

# Run: make clean
# if you need to update object files / the executable and the make file does not want to update. This can sometimes happen if only header files have been edited. 
clean:
	rm -f *.o
	rm -f $(EXECUTABLE)