#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int run = 1;

static void handler(int sig)
{
	switch (sig) {
	case SIGINT:
	case SIGTERM:
		/* restore default handlers */
		signal(SIGINT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);

		/* bye */
		run = 0;
		break;
	}
}

static int become_daemon(int do_chdir)
{
	errno = 0;

	int fd;
	long maxfd;

	if (do_chdir && chdir("/") < 0)
		return -1;

	/*
	 * close all open fd except stdin, stdout, stderr
	 * and reserve 1 fd
	 */
	maxfd = sysconf(_SC_OPEN_MAX);
	if (maxfd < 0)
		return -1;

	while (maxfd > 3) {
		(void)close(maxfd);
		maxfd--;
	}

	/* reset all signal handler to default */
	/* reset the signal mask, sigprocmask() */

	switch (fork()) {
	case -1:
		return -1;
	case 0:
		break;
	default:
		_exit(0);
	}

	if (setsid() < 0)
		return -1;

	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
		return -1;

	switch (fork()) {
	case -1:
		return -1;
	case 0:
		break;
	default:
		_exit(0);
	}

	umask(0);

	fd = open("/dev/null", O_RDWR);
	if (dup2(fd, 0) < 0 || dup2(fd, 1) < 0 || dup2(fd, 2) < 0) {
		close(fd);
		return -1;
	}

	if (fd > 2)
		close(fd);

	return 0;
}

int main(void)
{
	if (become_daemon(0) < 0)
		err(1, NULL);

	/* Handle SIGINT, SIGTERM */
	signal(SIGINT, handler);
	signal(SIGTERM, handler);

	while (run)
		sleep(1);

	return 0;
}
