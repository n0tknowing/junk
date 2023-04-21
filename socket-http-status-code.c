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

/* https://datatracker.ietf.org/doc/html/rfc2616#section-10 */
static char *status_str(int status)
{
	switch (status) {
	case 100:
		return "Continue";
	case 101:
		return "Switching Protocols";
	case 200:
		return "OK";
	case 201:
		return "Created";
	case 202:
		return "Accepted";
	case 203:
		return "Non-Authoritative Information";
	case 204:
		return "No Content";
	case 205:
		return "Reset Content";
	case 206:
		return "Partial Content";
	case 300:
		return "Multiple Choices";
	case 301:
		return "Moved Permanently";
	case 302:
		return "Found";
	case 303:
		return "See Other";
	case 304:
		return "Not Modified";
	case 305:
		return "Use Proxy";
	case 307:
		return "Temporary Redirect";
	case 400:
		return "Bad Request";
	case 401:
		return "Unauthorized";
	case 403:
		return "Forbidden";
	case 404:
		return "Not Found";
	case 405:
		return "Method Not Allowed";
	case 406:
		return "Not Acceptable";
	case 407:
		return "Proxy Authentication Required";
	case 408:
		return "Request Timeout";
	case 409:
		return "Conflict";
	case 410:
		return "Gone";
	case 411:
		return "Length Required";
	case 412:
		return "Precondition Failed";
	case 413:
		return "Request Entity Too Large";
	case 414:
		return "Request-URI Too Long";
	case 415:
		return "Unsupported Media Type";
	case 416:
		return "Requested Range Not Satisfiable";
	case 417:
		return "Expectation Failed";
	case 500:
		return "Internal Server Error";
	case 501:
		return "Not Implemented";
	case 502:
		return "Bad Gateway";
	case 503:
		return "Service Unavailable";
	case 504:
		return "Gateway Timeout";
	case 505:
		return "HTTP Version Not Supported";
	}

	return "Unknown Code";
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s host\n", argv[0]);
		return 1;
	}

	const char *host = argv[1], *hostt;
	const char *port = "80";

	if ((hostt = strstr(host, "//"))) {
		hostt += 2;
		while (*hostt == '/')
			hostt++;
		host = hostt;
	}

	ssize_t recv_ret;
	char buf[13], *code;
	int r = 0, fd = -1, status;
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
		if ((r = connect(fd, tmp->ai_addr, tmp->ai_addrlen)) < 0) {
			perror("connect");
			continue;
		}
		break;
	}

	if (!tmp) {
		fprintf(stderr, "failed to connect %s\n", host);
		return 1;
	}

	freeaddrinfo(res);

	/* alternative to snprintf + send */
	dprintf(fd, "HEAD / HTTP/1.1\r\n"
		    "Host: %s\r\n"
		    "User-Agent: dprintf/1.0\r\n"
		    "\r\n", host);

	if ((recv_ret = recv(fd, buf, 12, 0)) < 0) {
		perror("recv");
		return 1;
	}
	buf[recv_ret] = '\0';

	if (!(code = strchr(buf, ' '))) {
		fprintf(stderr, "%s: cannot found HTTP status code\n", host);
		fprintf(stderr, "returned: %s\n", buf);
		return 1;
	}

	status = atoi(code++);
	printf("%s: %d %s\n", host, status, status_str(status));

	close(fd);
	return 0;
}
