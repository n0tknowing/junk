CC=gcc
CFLAGS=-O2 -Wall -Wextra -Wpedantic -Wstrict-prototypes

all:
	$(CC) server.c -o sv $(CFLAGS)
	$(CC) client.c -o cl $(CFLAGS)

clean:
	rm -f sv cl
