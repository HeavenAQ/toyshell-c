shell := /bin/bash
CC = gcc
DEBUG = -g
CFLAGS = -Wall -Wextra -Werror
TARGET = shell
SRC_DIR = src
ELF_DIR = elf
SRC_FILES = main.o cmd.o shell.o utils.o

#====== clear suffix list ======
.SUFFIXES:
.SUFFIXES: .c .o

#====== default recipes ======
all: main.o cmd.o shell.o utils.o
	$(CC) $^ $(CFLAGS) -o $(TARGET)
	@[ -d $(ELF_DIR) ] || mkdir $(ELF_DIR) 2> /dev/null
	@mv *.o $(ELF_DIR)

%.o: $(SRC_DIR)/%.c
	$(CC) $< $(CFLAGS) -o $@ -c 

debug:
	$(CC) $(CFLAGS) $(DEBUG) -o $(TARGET) $(SRC_DIR)/*.c

clean:
	rm -f $(TARGET)
	rm -rf $(ELF_DIR)
	rm -rf *.o

.PHONY: all clean
