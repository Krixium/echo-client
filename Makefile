CC=gcc
CFLAGS=-Wall -ggdb
NAME=echoc.out
DEBUGNAME=echoc.out
LINKS=

default: net.o main.o
	$(CC) $(CFLAGS) net.o main.o -o $(NAME) $(LINKS)

debug: net.o main.o
	$(CC) $(CFLAGS) net.o main.o -ggdb -o $(DEBUGNAME) $(LINKS)

main.o:
	$(CC) $(CFLAGS) -O -c main.c

net.o:
	$(CC) $(CFLAGS) -O -c net.c

clean:
	rm -f *.o *.txt *.bak $(NAME) $(DEBUGNAME)