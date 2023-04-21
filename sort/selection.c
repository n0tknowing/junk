#define _POSIX_C_SOURCE	200809L
#include <stdio.h>

/* selection sort */
static void sort(int *arr, int size)
{
    if (!arr || !size)
        return;

    int i, j, min, tmp;

    for (i = 0; i < size; i++) {
        min = i;
        for (j = i + 1; j < size; j++)
            min = (arr[j] < arr[min]) ? j : min;
        tmp = arr[i];
        arr[i] = arr[min];
        arr[min] = tmp;
    }
}

int main(void)
{
    int a[] = {'E', 'A', 'S', 'Y', 'Q', 'U', 'E', 'S', 'T', 'I', 'O', 'N'};
    int len = sizeof a / sizeof *a;
    int i;

    dprintf(2, "before sorting: ");
    for (i = 0; i < len; i++)
        dprintf(2, "%c ", a[i]);

    dprintf(2, "\n");
    sort(a, len);

    dprintf(2, "after sorting: ");
    for (i = 0; i < len; i++)
        dprintf(2, "%c ", a[i]);
    dprintf(2, "\n");
}
