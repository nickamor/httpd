
SOURCES = $(shell find . .. -name "*.c" -maxdepth 1)
OBJECTS = $(SOURCES:.c=.o)

CC = clang
CFLAGS = -Wall -Wextra -g

all: $(OBJECTS)
	@$(CC) $? -o $(TARGET) $(LDFLAGS)	

%.o: %.c Makefile
	@$(CC) $(CFLAGS) -c -o $@ $<