#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static void die(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: prog binary-file\n");
		return 1;
	}

	FILE *fp = fopen(argv[1], "rb");
	if (!fp)
		die("cannot open file");

	fseek(fp, 0, SEEK_END);
	size_t l = ftell(fp);
	rewind(fp);

	int r = 0;
	unsigned char *buf = calloc(1, l);
	if (!buf) {
		perror("error allocating memory");
		r = 1;
		goto done;
	}

	size_t ret = fread(buf, sizeof(*buf), l, fp);
	if (ret != l) {
		printf("error reading binary file: ");
		if (ferror(fp))
			printf("ferror returned\n");
		if (feof(fp))
			printf("feof returned\n");
		r = 1;
		goto done2;
	}

	for (size_t i = 0; i < l; i++) {
		if (isprint(buf[i]))
			printf("%c \t 0x%04x \t %u\n", buf[i], buf[i], buf[i]);
		else
			printf("\t 0x%04x \t %u\n", buf[i], buf[i]);
	}
done2:
	free(buf);
done:
	fclose(fp);
	return r;
}
