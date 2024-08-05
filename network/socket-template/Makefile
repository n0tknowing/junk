CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -Wstrict-prototypes -O2

all: resolve

resolve: resolver.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f resolve
