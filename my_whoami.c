#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

int main(void)
{
	errno = 0;

	struct passwd *whoami = getpwuid(geteuid());
	if (!whoami) {
		perror("getpwuid");
		return 1;
	}

	printf("%s\n", whoami->pw_name);

	return 0;
}
