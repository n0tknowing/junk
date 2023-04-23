#include <ctype.h>
#include <stdio.h>

static int strnidx(const char *s, char c, size_t n)
{
	int i = 0;

	while (n--) {
		if (*s == c)
			return i;
		i++; s++;
	}

	return -1;
}

static int strncaseidx(const char *s, char c, size_t n)
{
	int i = 0;

	while (n--) {
		if (tolower(*s) == tolower(c))
			return i;
		i++; s++;
	}

	return -1;
}

static int stridx(const char *s, char c)
{
	int i = 0;

	while (*s) {
		if (*s == c)
			return i;
		i++; s++;
	}

	return -1;
}

static int strcaseidx(const char *s, char c)
{
	int i = 0;

	while (*s) {
		if (tolower(*s) == tolower(c))
			return i;
		i++; s++;
	}

	return -1;
}

int main(void)
{
	const char *s1 = "test\n\0o";

	printf("%d\n", strnidx(s1, 'o', 7));
	printf("%d\n", strncaseidx(s1, 'O', 7));
	printf("%d\n", stridx(s1, 't'));
	printf("%d\n", strcaseidx(s1, 'T'));

	return 0;
}
