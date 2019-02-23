CC=gcc
CFLAGS=-Wall -ggdb
NAME=echoc.out
DEBUGNAME=echoc.out
LINKS=-lpthread

default: main.o
	$(CC) $(CFLAGS) main.o -o $(NAME) $(LINKS)

debug: main.o
	$(CC) $(CFLAGS) main.o -ggdb -O0 -o $(DEBUGNAME) $(LINKS)

main.o:
	$(CC) $(CFLAGS) -O -c main.c

clean:
	rm -f *.o *.txt *.bak *.log $(NAME) $(DEBUGNAME)
