#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int my_strcmp(const char *s1, const char *s2)
{
	while (*s1++ == *s2++) {
		if (!*s1 && !*s2)
			return 0;
	}

	return s1 - s2;
}

static int strncasecmp(const char *s1, const char *s2, size_t n)
{
	size_t i = 0;
	while (i++ < n) {
		if (tolower(*s1++) != tolower(*s2++))
			return s1 - s2;
	}

	return 0;
}

static char *my_strstr(const char *str, const char *what)
{
	size_t l = strlen(what);
	while (*str) {
		if (!strncmp(str, what, l))
			return (char *)str;
		str++;
	}

	return NULL;
}

static char *my_strcasestr(const char *str, const char *what)
{
	size_t l = strlen(what);
	while (*str) {
		if (!strncasecmp(str, what, l))
			return (char *)str;
		str++;
	}

	return NULL;
}

int main(void)
{
	char *s = "hel=ooo";
	printf("%d\n", strncasecmp("hp", "HEL", 2));
	printf("%s\n", my_strstr(s, "l="));
	printf("%s\n", my_strcasestr(s, "l="));
	printf("%d\n", strncasecmp("Por", "poo", 3));
	printf("%d\n", my_strcmp("aaa", "aaa"));
	return 0;
}
