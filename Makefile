# Makefile for Blob.io

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces

# Raylib path (modify this if your Raylib installation is in a different location)
RAYLIB_PATH = C:/raylib/raylib

# Include paths
INCLUDES = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external

# Library paths
LDFLAGS = -L. -L$(RAYLIB_PATH)/src -L$(RAYLIB_PATH)/src/external

# Libraries
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Source files
SOURCES = jam1.c game.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = game

# Default target
all: $(EXECUTABLE)

# Linking
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

# Compiling
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

# Clean up
clean:
	del /Q $(EXECUTABLE).exe $(OBJECTS)

# Phony targets
.PHONY: all clean