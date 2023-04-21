#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc < 3)
		return 1;

	const char *ip = argv[1];
	in_addr_t ip4;
	in_port_t port4 = atoi(argv[2]);

	if (inet_pton(AF_INET, ip, &ip4) <= 0)
		err(1, "inet_pton");

	port4 = htons(port4);

	printf("%08x : %04x\n", ip4, port4);
	return 0;
}
