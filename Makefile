# Nick Amor - 2011

TARGETS = server-single server-forked server-threaded server-select

BINARIES = server-single server-forked server-threaded server-select

SRC = src/common.c src/config-file.c src/http.c src/sockets.c src/server-common.c
OBJ = $(SRC:.c=.o)
SRC_SINGLE = src/server-single/main.c
OBJ_SINGLE = $(SRC_SINGLE:.c=.o)
SRC_FORKED = src/server-forked/main.c src/server-forked/childpid.c
OBJ_FORKED = $(SRC_FORKED:.c=.o)
SRC_THREADED = src/server-threaded/main.c
OBJ_THREADED = $(SRC_FORKED:.c=.o)
SRC_SELECT = src/server-select/main.c
OBJ_SELECT = $(SRC_SELECT:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

.PHONY: all clean $(TARGETS)
all: $(TARGETS)

server-single: $(OBJ) $(OBJ_SINGLE)
	$(CC) $(OBJ) $(OBJ_SINGLE) -o server-single $(LDFLAGS)

server-forked: $(OBJ) $(OBJ_FORKED)
	$(CC) $(OBJ) $(OBJ_FORKED) -o server-forked $(LDFLAGS)

server-threaded: $(OBJ) $(OBJ_THREADED)
	$(CC) $(OBJ) $(OBJ_THREADED) -o server-threaded $(LDFLAGS)

server-select: $(OBJ) $(OBJ_SELECT)
	$(CC) $(OBJ) $(OBJ_SELECT) -o server-select $(LDFLAGS)

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) *.o **/*.o $(BINARIES)
