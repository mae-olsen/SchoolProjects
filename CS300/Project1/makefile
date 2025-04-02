# Makefile to compile each .c file separately

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall

# Sources and corresponding executables
SRCS = $(wildcard *.c)
EXES = $(SRCS:.c=)

# Default target
all: $(EXES)

# Rule to build each executable
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(EXES)
