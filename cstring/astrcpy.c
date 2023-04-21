#include <stdlib.h>
#include <string.h>

static char *astrncpy__(char **dst, const char *src, size_t size, int check)
{
    if (dst == NULL) {
        return NULL;
    } else if (src == NULL) {
        *dst = NULL;
        return NULL;
    }

    if (check) {
        const size_t src_len = strlen(src);
        size = (src_len < size) ? src_len : size;
    }

    *dst = calloc(size + 1, sizeof(**dst));
    if (*dst != NULL)
        memcpy(*dst, src, size);

    return *dst;
}

char *astrncpy(char **dst, const char *src, size_t size)
{
    return astrncpy__(dst, src, size, 1);
}

char *astrcpy(char **dst, const char *src)
{
    return astrncpy__(dst, src, src ? strlen(src) : 0, 0);
}
