/*
 * simple 'command -v' implementation.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h> /* errno */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int is_avail_exec(const char *cmd)
{
	if (!cmd)
		return 1;

	/*
	 * in libc, some functions are set errno if call to the function
	 * is failed and if there are two functions simoultaneously called
	 * at one time and the first function is failed, errno is set.
	 * 
	 * but, if the second function is also failed and errno is not set,
	 * a call perror() or strerror() to retrieve error string will
	 * using errno from the first failed function.
	 *
	 * this makes debugging hard because we got mysterious error.
	 *
	 * and in our case...
	 * snprintf() in function lookup() may overwrite errno and
	 * stat() is set errno too if call to the function failed.
	 *
	 * so, clear errno to avoid overwriting previous errno.
	 */
	errno = 0;

	struct stat s;
	int ret = stat(cmd, &s); /* stat() returns 0 on success */
	if (ret)
		return 1;

	return puts(cmd), 0;
}

static int lookup(const char *cmd)
{
	if (!cmd)
		return 1;

	/* check if 'cmd' is shell builtin */
	static const char *sh_builtins[] = {
		":", ".", "[", "alias", "bg", "break", "case", "cd", "command",
		"continue", "eval", "exec", "exit", "export", "fg", "getopts",
		"hash", "if", "jobs", "kill", "pwd", "read", "readonly",
		"return", "set", "shift", "test", "times", "trap", "type",
		"ulimit", "umask", "unalias", "unset", "until", "wait", "while"
	};
	int i, builtin_nums = sizeof(sh_builtins) / sizeof(sh_builtins[0]);

	for (i = 0; i < builtin_nums; i++) {
		if (!strcmp(sh_builtins[i], cmd)) {
			puts(cmd);
			return 0; /* 'cmd' is shell builtin */
		}
	}

	/* if we reached this, then 'cmd' is placed somewhere in
	 * 'PATH' environment */

	char buf[2096];
	char *path_env = getenv("PATH");
	if (!path_env)
		return 1;

	char *path = strtok(path_env, ":");
	if (!path)
		return 1;

	snprintf(buf, sizeof buf, "%s/%s", path, cmd);
	if (!is_avail_exec(buf))
		return 0;

	while ((path = strtok(NULL, ":"))) {
		memset(buf, 0, sizeof buf);
		snprintf(buf, sizeof buf, "%s/%s", path, cmd);
		if (!is_avail_exec(buf))
			return 0;
	}

	return 1;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	int ret;
	const char *cmd = argv[1];

	if (cmd[0] == '/')  /* absolute path */
		ret = is_avail_exec(cmd);
	else  /* relative path */
		ret = lookup(cmd);

	return ret;
}
