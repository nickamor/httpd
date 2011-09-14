
SOURCES = $(shell find . .. -name "*.c" -maxdepth 1)
OBJECTS = $(SOURCES:.c=.o)

HOST = $(shell uname)

CC = gcc
ifeq "$(HOST)" "Darwin"
	CC = clang
endif

CFLAGS = -Wall -Wextra -g

all: $(OBJECTS)
	@$(CC) $? -o $(TARGET) $(LDFLAGS)	

%.o: %.c Makefile
	@$(CC) $(CFLAGS) -c -o $@ $<