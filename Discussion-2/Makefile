CC=gcc
CFLAGS=-Wall -Werror -g

all: my-head

# $< refers to first dependency file
# $@ refers to the target name
my-head: my-head.o
	$(CC) $(CFLAGS) $< -o $@ 


my-head.o: my-head.c
	$(CC) $(CFLAGS) -c $< -o $@    

clean:
	rm -f my-head *.o

