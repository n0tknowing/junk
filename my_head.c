#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s file\n", argv[0]);
		return 1;
	}

	long maxline = 5;
	if (argv[2]) {
		maxline = strtol(argv[2], NULL, 10);
		if (!maxline) {
			fprintf(stderr, "Failed to convert to number\n");
			return 1;
		}
	}

	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		perror("Failed to open file");
		return 1;
	}

	char buf[1024];
	long curr_line = 0;
	while (fgets(buf, 1024, fp)) {
		if (strchr(buf, '\n')) {
			curr_line++;
		} else {
			fprintf(stderr, "String too long\n");
			break;
		}
		printf("%s", buf);
		if (curr_line == maxline)
			break;
	}

	fclose(fp);
	return 0;
}
