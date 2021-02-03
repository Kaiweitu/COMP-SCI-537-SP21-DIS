CC=gcc
CFLAGS=-Wall -Werror -g

all: my-head

my-head: my-head.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f my-head *.o

.PHONY: clean all
