/* a simple and buggy shell implementation */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CMD_BUF_SIZE 512

static const char *prog = "tinysh";
static const char *progver = "0.0.1-beta";
static int child_rc;

static void install_signal(int signum, void (*handler)(int))
{
	struct sigaction act = {0};

	act.sa_handler = handler;
	if (sigaction(signum, &act, NULL) < 0)
		err(1, "install_signal");
}

static int is_directory(const char *path)
{
	struct stat s;

	if (!stat(path, &s)) {
		if (S_ISREG(s.st_mode))
			return 0;
	}

	errno = EISDIR;
	return 1;
}

static int is_builtin(const char *cmd)
{
	static const char *builtins[] = {
		"exit", "false", "lastrc", "true"
	};
	int i, bn = sizeof(builtins) / sizeof(builtins[0]);

	for (i = 0; i < bn; i++) {
		if (!strcmp(builtins[i], cmd))
			return 1;
	}

	return 0;
}

static char *resolve_relative_path(const char *cmd)
{
	char *path_env, *path;
	char buf[CMD_BUF_SIZE + 1] = {0};
	int rc = -1;

	path_env = strdup(getenv("PATH"));
	if (!path_env)
		return NULL;

	path = strtok(path_env, ":");
	if (!path)
		goto bad;

	snprintf(buf, CMD_BUF_SIZE, "%s/%s", path, cmd);

	rc = access(buf, F_OK|X_OK);
	if (!rc) {
		free(path_env);
		return strdup(buf);
	}

	while ((path = strtok(NULL, ":"))) {
		snprintf(buf, CMD_BUF_SIZE, "%s/%s", path, cmd);
		rc = access(buf, F_OK|X_OK);
		if (!rc) {
			free(path_env);
			return strdup(buf);
		}
	}

bad:
	free(path_env);
	return NULL;
}

static char *lookup_cmd(const char *cmd)
{
	int rc;

	if (cmd[0] == '/') {
		rc = is_directory(cmd);
		if (!rc) {
			rc = access(cmd, F_OK|X_OK);
			if (!rc)
				return strdup(cmd);
		}
	} else {
		return resolve_relative_path(cmd);
	}

	return NULL;
}

static void builtin_exec(const char *cmd, const char *args)
{
	(void)args;

	if (!strcmp(cmd, "lastrc")) {
		dprintf(1, "last exit code: %d\n", child_rc);
		child_rc = 0;
	} else if (!strcmp(cmd, "true")) {
		child_rc = 0;
	} else if (!strcmp(cmd, "false")) {
		child_rc = 1;
	} else if (!strcmp(cmd, "exit")) {
		exit(0);
	}
}

static void child_exec(const char *cmd, const char *args)
{
	switch (fork()) {
	case -1:
		dprintf(2, "%s: %s: %s\n", prog, cmd, strerror(errno));
		return;
	case 0:
		execl(cmd, cmd, argv, NULL);
		dprintf(2, "%s: %s: %s\n", prog, cmd, strerror(errno));
		break;
	default:
		wait(&child_rc);
		break;
	}
}

static void print_help(void)
{
	dprintf(2, "Usage:\n");
	dprintf(2, "  %s [-hv] [-p PROMPT]\n", prog);
	dprintf(2, "\n");
	dprintf(2, "Options:\n");
	dprintf(2, "  -h           Show this message\n");
	dprintf(2, "  -p PROMPT    Use PROMPT instead of default prompt\n");
	dprintf(2, "  -v           Show software version\n");
}

int main(int argc, char **argv)
{
	const char *prompt = "user$ ";
	const char *errstr = "Command not found";
	char cmd[CMD_BUF_SIZE + 1], *path, *args = NULL;
	ssize_t readsz;
	int rc;

	while ((rc = getopt(argc, argv, "hp:v")) != -1) {
		switch (rc) {
		case 'h':
			print_help();
			return 0;
		case 'p':
			prompt = optarg;
			break;
		case 'v':
			dprintf(2, "%s -- tiny and buggy shell\n", prog);
			dprintf(2, "version %s\n", progver);
			return 0;
		default:
			print_help();
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	install_signal(SIGINT, SIG_IGN);
	install_signal(SIGQUIT, SIG_IGN);
	install_signal(SIGTSTP, SIG_IGN);

	while (1) {
		errno = 0;

		dprintf(1, "%s", prompt);

		readsz = read(0, cmd, CMD_BUF_SIZE);
		if (readsz == 1) {
			continue;
		} else if (readsz == 0) {
			dprintf(2, "\n");
			break;
		} else if (readsz == -1) {
			if (errno != EINTR)
				return errno;
			dprintf(1, "\n");
			child_rc = EINTR;
			continue;
		}

		cmd[strcspn(cmd, "\n")] = 0;
		cmd[readsz] = 0;

		args = strchr(cmd, ' ');
		if (args) {
			args++;
			cmd[(size_t)(args-cmd)-1] = 0;
		}

		rc = is_builtin(cmd);
		if (rc) {
			builtin_exec(cmd, args);
			continue;
		}

		path = lookup_cmd(cmd);
		if (!path) {
			if (errno != ENOENT)
				errstr = strerror(errno);
			dprintf(2, "%s: %s: %s\n", prog, cmd, errstr);
			child_rc = 0x7f;
			continue;
		}
		
		child_exec(path, args);
		free(path);
	}

	return 0;
}
