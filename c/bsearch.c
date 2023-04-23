#include <stdio.h>

static int bsearch(int *array, int length, int what)
{
	int low, mid, high, guess;

	low = 0;
	high = length - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		guess = array[mid];

		if (guess > what)
			low = mid + 1;
		else if (guess < what)
			high = mid - 1;
		else
			return mid;
	}

	return -1;
}

int main(void)
{
	int array[10] = {2, 10, 9, 8, 4, 29, 40, 51, 77, 3};
	printf("%d in array = %d\n", 10, bsearch(array, 10, 3));
	return 0;
}
