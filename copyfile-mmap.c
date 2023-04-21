#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static void copyfile(int fd, int targetfd, size_t fdsize)
{
	void *buf;

	buf = mmap(NULL, fdsize, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED)
		err(1, "copyfile (mmap)");

	if (write(targetfd, buf, fdsize) < 0) {
		munmap(buf, fdsize);
		err(1, "copyfile (write)");
	}

	munmap(buf, fdsize);
}

int main(int argc, char **argv)
{
	if (argc != 3)
		errx(1, "usage: %s file targetfile", argv[0]);

	int fd, tfd;
	struct stat st;

	fd = open(argv[1], O_RDONLY|O_CLOEXEC);
	if (fd < 0)
		err(1, "failed to open %s", argv[1]);

	if (fstat(fd, &st) < 0)
		err(1, "failed to get %s information", argv[1]);

	if (!S_ISREG(st.st_mode))
		errx(1, "%s is not a regular file", argv[1]);

	tfd = open(argv[2], O_WRONLY|O_CLOEXEC|O_CREAT, st.st_mode);
	if (tfd < 0)
		err(1, "failed to create %s", argv[2]);

	copyfile(fd, tfd, st.st_size);

	close(tfd);
	close(fd);
	return 0;
}
