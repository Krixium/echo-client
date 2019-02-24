CC=gcc
CFLAGS += -Wall -Werror
NAME=echoc.out
LINKS=-lpthread

SRC := main.c
OBJ := $(SRC:.c=.o)

.PHONY: default clean

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LINKS)

debug: $(OBJ)
	$(CC) $(CFLAGS) -ggdb -o $@ $^ $(LINKS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f *.o *.txt *.log $(NAME) $(DEBUGNAME)