#include <stdio.h>
#include <stdlib.h>

int *read_num(int *count)
{
	int mycount, n, size;
	int *a;

	mycount = 0; size = 1;

	a = malloc(sizeof(int) * size);
	if (a == NULL)
		return NULL;

	while (scanf("%d", &n) == 1) {
		while (mycount >= size) {
			size *= 2;
			a = realloc(a, sizeof(int) * size);
			if (a == NULL)
				return NULL;
		}
		a[mycount++] = n;
	}

	a = realloc(a, sizeof(int) * mycount);
	*count = mycount;

	return a;
}

int main(void)
{
	int b = 100;
	int *c = &b;
	int *a = read_num(c);
	free(a);
}
