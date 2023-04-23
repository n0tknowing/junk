#define _DEFAULT_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s url\n", argv[0]);
		return 1;
	}

	const char *name = argv[1];
	struct hostent *ns = gethostbyname(name);
	if (!ns) {
		herror("gethostbyname");
		return 1;
	}

	printf("%s\n", ns->h_name);
	for (int i = 0, l = ns->h_length / 2; i < l; i++) {
		struct in_addr addr;
		addr.s_addr = *((unsigned long *)ns->h_addr_list[i]);
		printf("   %s\n", inet_ntoa(addr));
	}

	return 0;
}
