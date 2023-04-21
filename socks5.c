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

#define SOCKS_VER	5
#define UP_AUTH_VER	1

#define REQ_CMD_CONNECT	1

#define REQ_ATYP_IPV4	1
#define REQ_ATYP_NAME	3

enum methods {
	NO_AUTH_METHOD  = 0,
	GS_AUTH_METHOD,
	UP_AUTH_METHOD,
	N_METHODS
};

enum replies {
	REP_SUCCESS = 0,  /* Succeeded */
	REP_SERVFAIL,   /* SOCKS server failure */
	REP_NOTALLOW,  /* Connection not allowed */
	REP_NETUNREACH,  /* Network unreachable */
	REP_HOSUNREACH,  /* Host unreachable */
	REP_CONNREF,   /* Connection refused */
	REP_TTLEXPR,  /* TTL Expired */
	REP_CMDFAIL,  /* Command not supported */
	REP_ADDRFAIL, /* Address type not supported */

};

struct socks_resp_t {
	unsigned char ver;
	unsigned char rep;
	unsigned char rsv;
	unsigned char atyp;
	union {
		char name[256];
		in_addr_t ip4;
	} addr;
	in_port_t port;
};

static const char *strreply(int reply)
{
	const char *repstr[] = {
		"Succeeded", "SOCKS server failure", "Connection not alllowed",
		"Network unreachable", "Host unreachable",
		"Connection refused", "TTL expired",
		"Command not supported", "Address type not supported"
	};

	if (reply >= REP_SUCCESS && reply <= REP_ADDRFAIL)
		return repstr[reply];

	return "Unknown reply";
}

static struct socks_resp_t *socks_request(int fd, const char *addr, const char *port)
{
	size_t off;
	in_addr_t req_addr;
	in_port_t req_port;
	struct socks_resp_t *resp;
	unsigned char req_buf[25], resp_buf[25];

	if (!addr || !*addr || !port || !*port)
		return NULL;

	off = 0;

	req_buf[off++] = SOCKS_VER;
	req_buf[off++] = REQ_CMD_CONNECT;
	req_buf[off++] = 0;
	req_buf[off++] = REQ_ATYP_IPV4;

	if (inet_pton(AF_INET, addr, &req_addr) <= 0)
		return NULL;

	memcpy(req_buf + off, &req_addr, 4); off += 4;
	req_port = htons(atoi(port));
	memcpy(req_buf + off, &req_port, 2); off += 2;

	if (send(fd, req_buf, off, 0) < 0)
		return NULL;

	if (recv(fd, resp_buf, sizeof(resp_buf), 0) < 0)
		return NULL;

	resp = malloc(sizeof(struct socks_resp_t));
	if (!resp)
		return NULL;

	off = 0;

	resp->ver = resp_buf[off++];
	resp->rep = resp_buf[off++];
	resp->rsv = resp_buf[off++];
	resp->atyp = resp_buf[off++];
	memcpy(&(resp->addr.ip4), resp_buf + off, 4); off += 4;
	memcpy(&resp->port, resp_buf + off, 2); off += 2;

	return resp;
}

static int socks_set_server(const char *host, const char *port)
{
	int fd;
	struct addrinfo hint, *res = NULL;

	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, port, &hint, &res) < 0) {
		errno = EADDRNOTAVAIL;
		return -1;
	}

	if ((fd = socket(res->ai_family, SOCK_STREAM, 0)) < 0)
		return -1;

	if (connect(fd, res->ai_addr, res->ai_addrlen) < 0)
		return -1;

	freeaddrinfo(res);
	return fd;
}

/*
 * -  STEP 1  Negotiation
 *
 *   Request format:
 *
 *           +-----+-----------+-----------+
 *           | VER | N_METHODS |  METHODS  |
 *           +-----+-----------+-----------+
 *           |  1  |     1     | 1 to 255  |
 *           +-----+-----------+-----------+
 *
 *  VER        =  0x05  The version of SOCKS protocol
 *  M_METHODS  =  0x01  The number of method identifier
 *  METHODS    =  The method identifier
 *               0x00  NO AUTHENTICATION REQUIRED
 *               0x01  GSSAPI
 *               0x02  USERNAME/PASSWORD
 *               0x03 to 0x7F  IANA ASSIGNED
 *               0x80 to 0xFE  RESERVED
 *               0xFF  NO ACCEPTABLE METHODS
 *
 *
 *   Response format:
 *
 *            +-----+--------+
 *            | VER | METHOD |
 *            +-----+--------+
 *            |  1  |   1    |
 *            +-----+--------+
 *
 *  VER        =  0x05  The Version of SOCKS protocol
 *  METHOD     =  One of METHODS in the Request format above
 */
static int socks_negotiate(int fd)
{
	int ret = -1;
	unsigned char req_buf[5], resp_buf[2];

	req_buf[0] = SOCKS_VER;
	req_buf[1] = N_METHODS;
	req_buf[2] = NO_AUTH_METHOD;
	req_buf[3] = GS_AUTH_METHOD;
	req_buf[4] = UP_AUTH_METHOD;

	if (send(fd, req_buf, 5, 0) < 0)
		goto notok;

	if (recv(fd, resp_buf, 2, 0) < 0)
		goto notok;

	ret = resp_buf[1];

notok:
	return ret;
}

static int socks_basic_auth(int fd, const char *user, const char *pass)
{
	int ret;
	size_t off;
	size_t ul, pl;
	unsigned char *req_buf, resp_buf[2];

	if (!user || !*user || !pass || !*pass)
		return -1;

	ret = 0;
	off = 0;
	ul = strlen(user);
	pl = strlen(pass);

	req_buf = malloc(3 + ul + pl);
	if (!req_buf)
		return -1;

	req_buf[off++] = UP_AUTH_VER;

	req_buf[off++] = ul;
	memcpy(req_buf + off, user, ul); off += ul;

	req_buf[off++] = pl;
	memcpy(req_buf + off, pass, pl); off += pl;

	if (send(fd, req_buf, off, 0) < 0) {
		ret = -1;
		goto notok;
	}

	if (recv(fd, resp_buf, 2, 0) < 0) {
		ret = -1;
		goto notok;
	}

	if (resp_buf[0] != 1 || resp_buf[1] != 0)
		ret = -1;

notok:
	free(req_buf);
	return ret;
}

static void http_get(int fd, const char *ip, const char *port)
{
	ssize_t ret, len, state;
	char buf[25];

	dprintf(fd, "GET / HTTP/1.1\r\n"
		    "Host: %s:%s\r\n"
		    "\r\n", ip, port);

	len = 0;
	state = 1;

	while (state) {
		ret = recv(fd, buf, 24, 0);
		if (ret != 24)
			state = 0;
		buf[ret] = 0;
		printf("%s", buf);
		len += ret;
	}

	printf("\n");
	printf("Total: %lu\n", len);
}

#ifndef SERVER
 #define SERVER	"51.68.190.205"
#endif

#ifndef PORT
 #define PORT	"8080"
#endif

/*
 * check ip: 193.122.130.0 port 80
 */

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s host port\n", argv[0]);
		return 1;
	}

	const char *host = argv[1];
	const char *port = argv[2];

	struct socks_resp_t *resp;
	char user[256], pass[256];
	int fd, method, ok = 0;

	if ((fd = socks_set_server(SERVER, PORT)) < 0)
		err(1, "socks_set_server");

	switch ((method = socks_negotiate(fd))) {
	case 0:
		printf("[OK] No authentication required\n");
		ok = 1;
		break;
	case 1:
		printf("GSSAPI authentication required\n");
		break;
	case 2:
		printf("User/Pass authentication required\n");

		printf("Username: "); fflush(stdout);
		if (fgets(user, 255, stdin) == NULL)
			errx(1, "Invalid user");
		user[strcspn(user, "\n")] = 0;

		printf("Password: "); fflush(stdout);
		if (fgets(pass, 255, stdin) == NULL)
			errx(1, "Invalid pass");
		pass[strcspn(pass, "\n")] = 0;

		if (socks_basic_auth(fd, user, pass) < 0) {
			printf("Authentication failed\n");
		} else {
			printf("Authentication OK\n");
			ok = 1;
		}
		break;
	default:
		printf("Invalid method: %d\n", method);
		break;
	}

	if (ok) {
		resp = socks_request(fd, host, port);
		if (!resp)
			err(1, "socks_request");

		if (resp->rep == REP_SUCCESS) {
			printf("Request OK\n\n");
			http_get(fd, host, port);
		} else {
			fprintf(stderr, "Request failed %s", strreply(resp->rep));
		}

		free(resp);
	}

	close(fd);
	return 0;
}
