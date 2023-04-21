#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		dprintf(2, "usage: %s host port\n", argv[0]);
		return 1;
	}

	const char *host = argv[1];
	const char *port = argv[2] ? argv[2] : "13";
	char buf[8];
	ssize_t r;

	struct sockaddr_in server = {
		.sin_family      = AF_INET,
		.sin_port        = htons(atoi(port)),
		.sin_addr.s_addr = inet_addr(host)
	};

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror(NULL);
		return 1;
	}

	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror(NULL);
		return 1;
	}

	while ((r = recv(sockfd, buf, 8, 0)) > 0) {
		buf[r] = 0;
		dprintf(2, "%s", buf);
	}

	close(sockfd);
	return 0;
}
