#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc < 2)
		errx(1, "usage: %s host port", argv[0]);

	int ret, fd;
	const char *host = argv[1], *port = argv[2]?argv[2]:"80";
	struct addrinfo *ai, *res, hint = {0};

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	printf("host = %s, port = %s\n", host, port);
	ret = getaddrinfo(host, port, &hint, &res);
	if (ret < 0) {
		if (ret == EAI_SYSTEM)
			err(1, "getaddrinfo");
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return 1;
	}

	for (ai = res; ai; ai = ai->ai_next) {
		fd = socket(ai->ai_family, SOCK_STREAM, 0);
		if (fd < 0) {
			perror("socket");
			continue;
		}
		if (connect(fd, ai->ai_addr, ai->ai_addrlen) == 0) {
			printf("Using %s connection\n", ai->ai_family == AF_INET
							? "IPv4"
							: "IPv6");
			break;
		}
		close(fd);
	}

	if (!ai)
		errx(1, "%d: failed to resolve %s", __LINE__, host);

	freeaddrinfo(res);

	close(fd);
	return 0;
}
