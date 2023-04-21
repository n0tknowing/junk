#include <stdio.h>

int main(void)
{
    const char *orig = "Hello world";
    printf("%.*s\n", 5, orig);
}
