#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "%s file\n", argv[0]);
		return 1;
	}

	struct stat st;

	if (stat(argv[1], &st) == -1) {
		perror("Failed to get file status");
		return 1;
	}

	printf("File: %s\n", argv[1]);
	printf("UID: %ld\n", (long)st.st_uid);
	printf("GID: %ld\n", (long)st.st_gid);
	printf("Size: %zu\n", (size_t)st.st_size);

	return 0;
}
