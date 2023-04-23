#include <stdio.h>

static int silly(int *x, int *y)
{
	*x = 0;
	*y = 1;
	return *x;
}

static int silly2(int *restrict x, int *restrict y)
{
	*x = 0;
	*y = 1;
	return *x;
}

int main(void)
{
	int a, b;

	a = silly(&a, &a);
	printf("silly(): %d:%d\n", a, a);

	// error: silly2(&b, &b);
	// error: silly2(&a, &a);
	(void)silly2(&b, &a);
	printf("silly2(): %d:%d\n", a, b);

	return 0;
}
