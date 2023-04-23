#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int
vasprintf(char **out, const char *fmt, va_list va)
{
    int r;
    va_list ap;
    char *str = NULL;

    va_copy(ap, va);

#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    r = vasprintf(&str, fmt, ap);
  #if defined(__GLIBC__)
    if (r < 0)
        str = NULL;
  #endif
#else
    r = vsnprintf(NULL, 0, fmt, ap);
    if (r < 0)
        goto done;

    str = calloc((size_t)r + 1UL, sizeof(*str));
    if (str == NULL) {
        r = -1;
        goto done;
    }

    r = vsnprintf(str, (size_t)r + 1UL, fmt, ap);
    if (r < 0) {
        free(str);
        str = NULL;
    }

done:
#endif
    va_end(ap);
    *out = str;
    return r;
}

int __attribute__((format(printf, 2, 3)))
asprintf(char **out, const char *fmt, ...)
{
    int r;
    va_list ap;

    va_start(ap, fmt);
    r = vasprintf(out, fmt, ap);
    va_end(ap);

    return r;
}
