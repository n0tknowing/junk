#include <stdio.h>
#include <string.h>

static int str_starts_with(const char *s1, const char *s2)
{
	while (*s2 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s2 == '\0';
}

static int str_ends_with(const char *s1, const char *s2)
{
	int s1len = strlen(s1);
	int s2len = strlen(s2);
	if (s2len > s1len)
		return 0;
	int off = s1len - s2len;
	return !strncmp(s1 + off, s2, s2len);
}

int main(int argc, char **argv)
{
	int i;
	const char *str, *prefix;

	if (argc < 2)
		return 1;

	prefix = argv[1];

	for (i = 2; i < argc; i++) {
		str = argv[i];
		if (str_starts_with(str, prefix))
			printf("[%s] starts with %s\n", str, prefix);
		if (str_ends_with(str, prefix))
			printf("[%s] ends with %s\n", str, prefix);
	}

	return 0;
}
