CC = gcc
CFLAGS = -Wall -gstabs -lpanel -lncurses

chex: chex.c display.c route.c buffer.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm chex
