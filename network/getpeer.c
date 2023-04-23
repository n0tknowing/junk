/* getpeername() returns IP and port that sucessfuly connect to a server */

#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s host\n", argv[0]);
		return 1;
	}

	const char *host = argv[1], *hostt;
	const char *port = argv[2] ? argv[2] : "80";

	if ((hostt = strstr(host, "//"))) {
		hostt += 2;
		while (*hostt == '/')
			hostt++;
		host = hostt;
	}

	char *ip = NULL;
	socklen_t addr_len;
	int r = 0, fd = -1;
	struct sockaddr_in addr;
	struct addrinfo hint, *tmp, *res = NULL;

	memset(&hint, 0, sizeof(hint));
	memset(&addr, 0, sizeof(addr));

	addr_len = sizeof(addr);

	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	if ((r = getaddrinfo(host, port, &hint, &res)) < 0) {
		fprintf(stderr, "%s: %s\n", host, gai_strerror(r));
		return 1;
	}

	for (tmp = res; tmp; tmp = tmp->ai_next) {
		if ((fd = socket(tmp->ai_family, tmp->ai_socktype, 0)) < 0) {
			perror("socket");
			continue;
		}

		if ((r = connect(fd, tmp->ai_addr, tmp->ai_addrlen)) != -1)
			break;

		perror("connect");
		close(fd);
	}

	if (!tmp) {
		fprintf(stderr, "failed to connect %s\n", host);
		return 1;
	}

	freeaddrinfo(res);

	if (getpeername(fd, (struct sockaddr *)&addr, &addr_len) < 0)
		err(1, "getpeername");

	ip = inet_ntoa(addr.sin_addr);
	printf("You're connected to %s:%d\n", ip, ntohs(addr.sin_port));

	close(fd);
	return 0;
}
