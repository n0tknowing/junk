#define _POSIX_C_SOURCE 200809L
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
	const char *port = "1965";

	if ((hostt = strstr(host, "//"))) {
		hostt += 2;
		while (*hostt == '/')
			hostt++;
		host = hostt;
	}

	ssize_t recv_r;
	char recv_buf[25];
	int r = 0, fd = -1;
	struct addrinfo hint, *tmp, *res = NULL;

	memset(&hint, 0, sizeof(hint));

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

	printf("%s: successfuly connected to port %s\n", host, port);
	dprintf(fd, "gemini://%s/\r\n", host);

	ssize_t tot = 0;

	while ((recv_r = recv(fd, recv_buf, 24, 0)) > 0) {
		recv_buf[recv_r] = '\0';
		tot += recv_r;
		printf("%s", recv_buf);
	}

	printf("\nRetrieved %lu bytes\n", tot);

	close(fd);
	return 0;
}
