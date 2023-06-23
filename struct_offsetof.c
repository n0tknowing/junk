#include <stddef.h>
#include <assert.h>

typedef unsigned char uchar;

struct foo {
    int a;
    int b;
};

int main(void)
{
    struct foo foo;

    *((uchar *)&foo + offsetof(struct foo, a)) = 1;
    *((uchar *)&foo + offsetof(struct foo, b)) = 2;

    assert(foo.a == 1);
    assert(foo.b == 2);
}
