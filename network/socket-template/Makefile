CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -Wstrict-prototypes -O2 -D_WANT_TEST

all: resolve

resolve: resolver.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f resolve
