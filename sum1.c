#include <stdio.h>

/* O(1) */
int sum1(int n)
{
	return n*(n-1)/2;
}

int main(void)
{
	printf("%d\n", sum1(10000));

	return 0;
}
