CC=gcc
CFLAGS=-Wall -ggdb
NAME=echoc.out
DEBUGNAME=echoc.out
LINKS=

default: main.o
	$(CC) $(CFLAGS) main.o -o $(NAME) $(LINKS)

debug: main.o
	$(CC) $(CFLAGS) main.o -ggdb -o $(DEBUGNAME) $(LINKS)

main.o:
	$(CC) $(CFLAGS) -O -c main.c

clean:
	rm -f *.o *.txt *.bak $(NAME) $(DEBUGNAME)