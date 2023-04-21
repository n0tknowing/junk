#include <stdio.h>

/* brute force */
static int max_el(int a[], int n)
{
	int i = 0;

	if (n < 0)
		return 0;

	for (i = 1; i < n; i++)
		if (a[0] < a[i])  /* min_el: a[0] > a[i] */
			a[0] = a[i];

	return a[0];
}

int main(void)
{
	int a[5] = {99,98,123,134,21};
	printf("%d\n", max_el(a, 5));

	return 0;
}
