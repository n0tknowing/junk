#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* my implementation of printenv(1) */
int main(int argc, char **argv)
{
	extern char **environ;

	if (!argv[1]) {
		if (environ) {
			for (; *environ; environ++)
				puts(*environ);
		}
	} else {
		char *env;
		for (int i = 1; i < argc; i++) {
			if ((env = getenv(argv[i])))
				puts(env);
			else
				return 1;
		}
	}

	return 0;
}
