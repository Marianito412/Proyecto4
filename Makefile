# Compiler
CC = gcc

# Compiler flags for GTK+3 and Glade
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c11
LIBS = `pkg-config --libs gtk+-3.0` -lm

# Executable name
APPNAME = main

# Source and object files
SRC = $(APPNAME).c
OBJ = $(SRC:.c=.o)

# Default target
all: $(APPNAME)

# Link the object file into an executable
$(APPNAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile .c to .o
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
.PHONY: clean run

clean:
	rm -f $(OBJ) $(APPNAME)

# Run the application
run: $(APPNAME)
	./$(APPNAME)
