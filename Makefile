shell := /bin/bash
CC = gcc
DEBUG = -g
CFLAGS = -Wall -Wextra -Werror
TARGET = shell

#====== clear suffix list ======
.SUFFIXES:
.SUFFIXES: .c .o

#====== default recipes ======
all: cmd.o shell.o main.o utils.o
	$(CC) $^ $(CFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $< $(CFLAGS) -o $@ -c 

debug:
	$(CC) $(CFLAGS) $(DEBUG) -o $(TARGET) *.c

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
