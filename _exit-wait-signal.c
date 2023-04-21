#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	int fd, child, ret, status = 0;

	if ((fd = open("file.txt", O_RDWR)) < 0)
		err(1, NULL);

	printf("FD = %d\n", fd);

	if ((child = fork()) < 0) {
		err(1, NULL);
	} else if (child == 0) {
		printf("--- Child write()-ing...\n");
		if (write(fd, "Child ", 6) < 0) {
			perror("--- child's write(2)");
			_exit(1);
		}
		dprintf(fd, "%d\n", getpid());
		printf("--- Child done write()-ing\n");
	} else {
		printf("-- wait()-ing...\n");
		if ((ret = wait(&status)) < 0)
			err(1, "wait()");
		printf("-- Done wait()-ing\n");
		if (WIFEXITED(status))
			printf("Child was terminated normally\n");
		else if (WIFSIGNALED(status))
			printf("Child was terminated by a signal\n");
		else 
			printf("Child was terminated by ????\n");
		printf("Child status: %d\n", WEXITSTATUS(status));
		printf("-- Parent write()-ing...\n");
		if (write(fd, "Parent ", 7) < 0)
			err(1, NULL);
		dprintf(fd, "%d\n", getpid());
		printf("-- Parent done write()-ing\n");
		printf("All OK!\n");
	}

	close(fd);
	return 0;
}
