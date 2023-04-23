#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	/* function to such a pointer returning int */
	int (*echo)(const char *, ...);
	/* function to such a pointer returning size_t */
	size_t (*len)(const char *);

	echo = printf;
	len = strlen;

	char *hw = "hello world";
	echo("%s length is %zu\n", hw, len(hw));

	return EXIT_SUCCESS;
}
