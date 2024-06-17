# Compiler and Compiler Flags
CC=gcc
CFLAGS=-Wall -g -Iinclude
# Linker flags
LDFLAGS=-lreadline

# The build target executable:
TARGET=minios

# Source, Object files
SRCS=kernel/kernel.c kernel/system.c kernel/ls.c kernel/cd.c kernel/mkdir.c kernel/touch.c
OBJS=$(SRCS:.c=.o) 

# Include directory
INCLUDE_DIR=include/system.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# To obtain object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Back up
backup:
	gcc -o ./kernel/20182672/backup ./kernel/20182672/main.c

# Clean up:
clean:
	rm -f ./kernel/20182672/backup
	rm -f $(OBJS) $(TARGET)
