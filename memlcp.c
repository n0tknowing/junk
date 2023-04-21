#include <stdint.h>
#include <string.h>

static size_t memlcp(const void *s, const void *r, const size_t len)
{
    const uint8_t *a = s;
    const uint8_t *b = r;
    size_t i = 0UL;

    while (i < (len & ~(sizeof(uintptr_t) - 1UL))) {
        if (memcmp(a + i, b + i, sizeof(uintptr_t) - 1UL) != 0)
            break;
        i += 8UL;
    }

    while (i < len) {
        if (a[i] != b[i])
            break;
        i += 1;
    }

    return i;
}
