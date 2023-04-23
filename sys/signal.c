#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#ifndef __unused
 #define __unused   __attribute((unused))
#endif

static int state = 1;

static void install_signal(int signum, void (*handler)(int))
{
	struct sigaction act;

	act.sa_handler = handler;
	if (sigaction(signum, &act, NULL) < 0)
		err(1, "install_signal");
}

static void sigint_handler(int sig __unused)
{
	fprintf(stderr, "Receive SIGINT....\n");
	fprintf(stderr, "Bye bye...\n");
	state = 0;
}

static void sigterm_handler(int sig __unused)
{
	fprintf(stderr, "Receive SIGTERM....\n");
	state = 0;
}

static void sigalrm_handler(int sig __unused)
{
	fprintf(stderr, "Timeout...\n");
	state = 0;
}

/* static void sigkill_handler(int sig)
{
	UNUSED(sig);

	fprintf(stderr, "Receive SIGKILL\n");
	state = 0;
} */

int main(void)
{
	install_signal(SIGINT, sigint_handler);
	install_signal(SIGTERM, sigterm_handler);
	install_signal(SIGALRM, sigalrm_handler);
//	install_signal(SIGKILL, sigkill_handler); // safely compiled but error

	alarm(60);

	while (state);

	return 0;
}
