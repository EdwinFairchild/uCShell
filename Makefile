# Define the compiler
CC=arm-none-eabi-gcc

# Compiler flags
CFLAGS=-c -Wall

# Archive tool for creating static library
AR=arm-none-eabi-ar

# Target library name
LIB_NAME=libmylibrary.a

# Find all .c files in the directory
SRC=$(wildcard *.c)

# Substitute .c files with .o files
OBJ=$(SRC:.c=.o)

# Default make target
all: $(LIB_NAME)

# Rule to make the static library
# Rule to make the static library
$(LIB_NAME): $(OBJ)
	$(AR) rcs $@ $(OBJ)


# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean the build
clean:
	rm -f $(OBJ) $(LIB_NAME)
