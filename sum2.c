#include <stdio.h>

/* O(n) */
int sum2(int n)
{
	int i, sum = 0;

	for (i = 0; i < n; i++)
		sum += i;

	return sum;
}

int main(void)
{
	printf("%d\n", sum2(10000));

	return 0;
}
