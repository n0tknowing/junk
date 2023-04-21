/*
 * Pointer aliasing
 *
 * bacaan lebih lanjut:
 * - https://en.wikipedia.org/wiki/Aliasing_(computing)
 * - https://cvw.cac.cornell.edu/vector/coding_aliasing
 * - https://stackoverflow.com/a/36795782/14092669
 */
#include <stdio.h>
#include <stdlib.h>

int *foo(int *prev, int val)
{
    int *n = prev;  // ALIASING
    *n = val;
    return prev;  // prev juga keubah....
}

int main(void)
{
    int *k = calloc(1, sizeof(int));
    if (!k)
        return 1;

    printf("k = %d\n", *k);
    k = foo(k, 9);
    printf("k = %d\n", *k);
    k = foo(k, 90);
    printf("k = %d\n", *k);
    k = foo(k, 900);
    printf("k = %d\n", *k);
    k = foo(k, 9000);
    printf("k = %d\n", *k);

    free(k);
    return 0;
}

/* Contoh lainnya adalah swap 2 variabel */
