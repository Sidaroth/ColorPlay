# The compiler
CC=g++
# Flags for the compiler
CFLAGS=-c -std=gnu++11 -Wall
# Flags for the linker. 
LFLAGS=-lcurl
# Name of the executable
EXECUTABLE=ColorPlay
# Source files
SOURCES=main.cpp LogModule.cpp StringQueue.cpp BulbHandler.cpp
# The objects
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LFAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@