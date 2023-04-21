#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help(void)
{
	printf("Usage: seq FIRST INC LAST\n\n");
	printf("Print numbers from FIRST to LAST, in steps of INC.\n\n");
	printf("If only one argument is supplied, FIRST and INC is set to 1.\n");
	printf("If two argument is supplied, INC is set to 1.\n");
	printf("Otherwise, all is set from arguments.\n");
}

/* seq(1) */
int main(int argc, char **argv)
{
	if (argc < 1) {
		print_help();
		return 1;
	}

	long long first, inc, last;
	inc = 1;
	errno = 0;

	switch (argc) {
	case 2:
		last = strtoll(argv[1], NULL, 10);
		if (errno == 0) {
			for (first = 1; first <= last; first += inc)
				printf("%lld\n", first);
		} else {
			fprintf(stderr, "my_seq: %s\n", strerror(errno));
			return 1;
		}
		break;
	case 3:
		first = strtoll(argv[1], NULL, 10);
		last = strtoll(argv[2], NULL, 10);
		if (errno == 0) {
			for (; first <= last; first += inc)
				printf("%lld\n", first);
		} else {
			fprintf(stderr, "my_seq: %s\n", strerror(errno));
			return 1;
		}
		break;
	case 4:
		inc = strtoll(argv[2], NULL, 10);
		first = strtoll(argv[1], NULL, 10);
		last = strtoll(argv[3], NULL, 10);
		if (errno == 0) {
			for (; first <= last; first += inc)
				printf("%lld\n", first);
		} else {
			fprintf(stderr, "my_seq: %s\n", strerror(errno));
			return 1;
		}
		break;
	default:
		print_help();
		return 1;
	}

	return 0;
}
