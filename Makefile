CC = gcc
CFLAGS = -Wall -gstabs -lpanel -lncurses -pedantic -std=c99

chex: chex.c view.c route.c buffer.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm chex
