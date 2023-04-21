#include <stdio.h>

/* sigma notation and for loop
 * summation
 *
 *   6
 *  ----
 *  \    i^2 = 3^2 + 4^2 + 5^2 + 6^2 = 86
 *  /___
 *  i = 3
 *  
 */
int main(void)
{
    int sum = 0;

    for (int i = 3; i <= 6; i++)
        sum = i * i + sum;

    printf("%d\n", sum);
}
