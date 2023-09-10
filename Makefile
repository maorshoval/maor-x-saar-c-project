# Compiler
CC = gcc

# Compiler flags
CFLAGS = -ansi -Wall -pedantic

# Source files
SRCS = main.c macro.c get_line_info.c convertion_phase.c create_output.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = assembler

# Default target: build the executable
all: $(EXEC)
# Compile and link the program
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean generated files
clean:
	rm -f $(OBJS) $(EXEC)

