# Makefile for compiling shell.c and scheduler.c

CC = gcc
CFLAGS = -Wall -g

all: shell

shell: shell.o scheduler.o
	$(CC) $(CFLAGS) -o shell shell.o scheduler.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c

scheduler.o: scheduler.c
	$(CC) $(CFLAGS) -c scheduler.c

clean:
	rm -f *.o shell
