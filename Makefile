# Nick Amor - 2011

TARGETS = server-single server-forked server-threaded

BINARIES = server-single/server-single slow-gethttp/slow-gethttp \
		server-forked/server-forked server-threaded/server-threaded \
		server-select/server-select

SRC = common.c config-file.c http.c sockets.c server-common.c
OBJ = $(SRC:.c=.o)
SRC_SINGLE = server-single/main.c
OBJ_SINGLE = $(SRC_SINGLE:.c=.o)
SRC_FORKED = server-forked/main.c server-forked/childpid.c
OBJ_FORKED = $(SRC_FORKED:.c=.o)
SRC_THREADED = server-threaded/main.c
OBJ_THREADED = $(SRC_FORKED:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

include makefile.platform

.PHONY: all clean $(TARGETS)
all: $(TARGETS)

server-single: $(OBJ) $(OBJ_SINGLE)
	@$(CC) $(OBJ) $(OBJ_SINGLE) -o server-single/server-single $(LDFLAGS)

server-forked: $(OBJ) $(OBJ_FORKED)
	@$(CC) $(OBJ) $(OBJ_FORKED) -o server-forked/server-forked $(LDFLAGS)

server-threaded: $(OBJ) $(OBJ_THREADED)
	@$(CC) $(OBJ) $(OBJ_THREADED) -o server-threaded/server-threaded $(LDFLAGS)

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) *.o **/*.o $(BINARIES)
