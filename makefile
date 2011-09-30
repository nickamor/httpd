# Nick Amor - 2011

BINARIES = server-single/server-single slow-gethttp/slow-gethttp \
		server-forked/server-forked server-threaded/server-threaded \
		server-select/server-select

SRC = common.c config-file.c http.c sockets.c
OBJ = $(SRC:.c=.o)
SRC_SINGLE = server-single/main.c
OBJ_SINGLE = $(SRC_SINGLE:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

include makefile.platform

.PHONY: all clean server-single
all: server-single

server-single: $(OBJ) $(OBJ_SINGLE)
	@$(CC) $(OBJ) $(OBJ_SINGLE) -o server-single/server-single $(LDFLAGS)

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) *.o **/*.o $(BINARIES)
