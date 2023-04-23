#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc < 1)
		return 1;

	const char *file;
	struct stat st;

	file = argv[1];
	if (stat(file, &st) < 0) {
		perror("stat");
		return 1;
	}

	printf("block size: %lu\n", st.st_blksize);

	return 0;
}
