#include <stdio.h>

int main(void)
{
    char *q = "hello world";
    int c;

//    while ((c = *q++) != '\0') {
//        printf("c  = %c,   q = %c\n", c, *q);
//    }

    // or

    while (*q != '\0') {
        c = *q;
        q += 1;
        printf("c  = %c,   q = %c\n", c, *q);
    }
}
