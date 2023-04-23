#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

static void sigchld(int signum)
{
	fprintf(stderr, "Receiving SIGCHLD...\n");
	fprintf(stderr, "Hmmmmmmmmmmm.....\n");
}

int main(void)
{
	int fd, child, ret, status = 0;
	struct sigaction act;

	sigemptyset(&act.sa_mask);

	act.sa_handler = SIG_IGN;
	if (sigaction(SIGCHLD, &act, NULL) < 0)
		err(1, "sigaction");

	if ((fd = open("file.txt", O_RDWR)) < 0)
		err(1, NULL);

	printf("FD = %d\n", fd);

	if ((child = fork()) < 0) {
		err(1, NULL);
	} else if (child == 0) {
		printf("--- Child write()-ing...\n");
		if (write(fd, "Child\n", 6) < 0) {
			perror("--- child's write(2)");
			_exit(1);
		}
		printf("--- Child done write()-ing\n");
	} else {
		printf("-- wait()-ing...\n");
		if ((ret = wait(&status)) < 0)
			err(1, "wait()");
		printf("-- Done wait()-ing\n");
		if (WIFEXITED(status))
			printf("Child terminated normally\n");
		printf("Child status: %d\n", WEXITSTATUS(status));
		printf("-- Parent write()-ing...\n");
		if (write(fd, "Parent\n", 7) < 0)
			err(1, NULL);
		printf("-- Parent done write()-ing\n");
		printf("All OK!\n");
	}

	close(fd);
	return 0;
}
