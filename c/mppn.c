#include <stdio.h>

#define MAX(x, y)	((x) > (y) ? (x) : (y))

int mppn(int *a, int n)
{
	int prod = 0;
	int i, j;

	for (i = 1; i < n; i++) {
		for (j = i + 1; j < n; j++) {
			prod = MAX(prod, a[i]*a[j]);
		}
	}

	return prod;
}

int main(void)
{
	int a[] = {7, 5, 14, 2, 8, 8, 10, 1, 2, 3};
	int n = 10;
	printf("%d\n", mppn(a, n));

	return 0;
}
