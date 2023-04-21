#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define N_CHARS	 (UCHAR_MAX + 1)

/* print a lowecase vowel twice */
int putchar_twice(int c)
{
	if (putchar(c) == EOF || putchar(c) == EOF) {
		return EOF;
	} else {
		return c;
	}
}

int main(void)
{
	/* an array of function pointers */
	int (*table[UCHAR_MAX + 1])(int);
	int i, c;

	for (i = 0; i < UCHAR_MAX; i++)
		table[i] = putchar;

	table['a'] = putchar_twice;
	table['e'] = putchar_twice;
	table['i'] = putchar_twice;
	table['o'] = putchar_twice;
	table['u'] = putchar_twice;

	while ((c = getchar()) != EOF)
		table[c](c);

	return EXIT_SUCCESS;
}
