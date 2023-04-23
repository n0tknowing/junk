#include <stdio.h>

/* Tail recursion is when a recursive function calls itself only once,
 * and as the last thing it does. */
static void print_range_r(int start, int stop)
{
	/* start will grow up and stop when start >= stop */
	if (start < stop) {
		printf("%d\n", start);
		print_range_r(start + 1, stop);
	}
}

int main(void)
{
	print_range_r(1, 11);

	return 0;
}
