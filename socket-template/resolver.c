#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

const char *xgai_err(int rc)
{
	const char *errstr;

	if (rc == EAI_SYSTEM)
		errstr = strerror(errno);
	else
		errstr = gai_strerror(rc);

	return errstr;
}

void xresolve(const char *restrict node, const char *restrict service,
	      const struct addrinfo *restrict hints,
	      struct addrinfo **restrict res)
{
	const char *errstr;
	int rc;

	rc = getaddrinfo(node, service, hints, res);
	if (rc < 0) {
		errstr = xgai_err(rc);
		dprintf(2, "xresolve: %s\n", errstr);
		exit(1);
	}
}

void xaddrtostr(const struct sockaddr *restrict addr, socklen_t addrl,
		char *restrict host, socklen_t hostl,
		char *restrict serv, socklen_t servl)
{
	const char *errstr;
	int rc;

	rc = getnameinfo(addr, addrl, host, hostl, serv, servl,
			 NI_NUMERICHOST | NI_NUMERICSERV);
	if (rc < 0) {
		errstr = xgai_err(rc);
		dprintf(2, "xaddrtostr: %s\n", errstr);
		exit(1);
	}
}

#ifdef _WANT_TEST
int main(int argc, char **argv)
{
	const char *prog = argv[0];
	char ipstr[128] = {0};
	struct addrinfo hints, *addr;
	const char *host, *port;
	int rc, af = AF_UNSPEC, st = SOCK_STREAM;

	while ((rc = getopt(argc, argv, ":46u")) != -1) {
		switch (rc) {
		case '4':
			af = AF_INET;
			break;
		case '6':
			af = AF_INET6;
			break;
		case 'u':
			st = SOCK_DGRAM;
			break;
		default:
			die:
			dprintf(2, "usage: %s [-4|6] [-u] host [port]\n", prog);
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 1)
		goto die;

	host = argv[0];
	port = argv[1] ? argv[1] : NULL;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = af;
	hints.ai_socktype = st;

	xresolve(host, port, &hints, &addr);
	xaddrtostr(addr->ai_addr, addr->ai_addrlen, ipstr, 128, NULL, 0);

	dprintf(1, "%s\n", ipstr);

	return 0;
}
#else
int main(void)
{
	return 0;
}
#endif
