#include <stdio.h>

/*
 * illustration for quick sort
 *
 * pivot is from the first element
 *
 * array = [5 1 7 3 10]
 * pivot = 5
 *
 * array = [1 3] [5] [7 10]
 * pivot =   1         7
 *
 * array = [1] [3] [5] [7] [10]
 *
 * if an array has length less than 2 that meant "divide" processs is done
 *
 * "conquer" proccess
 * array = [1 3 5 7 10]
 *
 * OK
 *
 */

#define swap(x, y) do { \
    int tmp = x;	    \
    x = y;		        \
    y = tmp;	        \
} while (0)

static void qs(int *arr, int n)
{
    if (n < 2)
        return;

    int pivot = arr[0];
    int left, right;

    left = 0;
    right = n - 1;

    while (left < right) {
        while (arr[left] < pivot && left < right)
            left++;
        while (arr[right] > pivot && left < right)
            right--;
        if (left < right && arr[left] != arr[right]) {
            if (arr[left] == pivot)
                pivot = arr[right];
            else if (arr[right] == pivot)
                pivot = arr[left];
            swap(arr[left], arr[right]);
        } else {
            left++;
        }
    }

    qs(arr, left - 1); /* less than pivot */
    qs(arr + left, n - left); /* greater than pivot */
}

int main(void)
{
    int a[5] = {5,1,7,3,10};
    int i;

    for (i = 0; i < 5; i++)
        printf("%d ", a[i]);

    printf("\n");

    qs(a, 5);

    for (i = 0; i < 5; i++)
        printf("%d ", a[i]);

    printf("\n");
}
