#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		dprintf(2, "usage: %s port host\n", argv[0]);
		return 1;
	}

	const char *port = argv[1] ? argv[1] : "13";
	const char *host = argv[2] ? argv[2] : "0.0.0.0";
	char buf[100];
	int clientfd, r, tot = 0;
	time_t t;
	struct sockaddr_in client = {0};
	socklen_t client_l = sizeof(client);

	struct sockaddr_in server = {
		.sin_family      = AF_INET,
		.sin_port        = htons(atoi(port)),
		.sin_addr.s_addr = inet_addr(host)
	};

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd < 0) {
		perror(NULL);
		return 1;
	}

	int ok = 1;
	setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(ok));

	if (bind(serverfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror(NULL);
		return 1;
	}

	if (listen(serverfd, 5) < 0) {
		perror(NULL);
		return 1;
	}

	dprintf(2, "listening on %s:%s....\n\n", host, port);

	while (1) {
		clientfd = accept(serverfd, (struct sockaddr *)&client, &client_l);
		if (clientfd < 0) {
			perror(NULL);
			break;
		}
		t = time(NULL);
		r = snprintf(buf, 100, "%.24s\r\n", ctime(&t));
		if (send(clientfd, buf, (size_t)r, 0) < 0) {
			perror(NULL);
			break;
		}
		dprintf(2, "Got connection from %s\n", inet_ntoa(client.sin_addr));
		dprintf(2, "[%d:%d] successfuly sent %s", tot++, clientfd, buf);
		close(clientfd);
		sleep(1);
	}

	close(serverfd);
	return 0;
}
