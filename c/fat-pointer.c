#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const size_t STR_MAX_SIZE = UINT32_C(536870911); /* 512 MiB */
typedef char* str;

str str_newlen(const char *ptr, size_t len)
{
    if (len > STR_MAX_SIZE)
        abort();
    str s = calloc(sizeof(uint32_t) + len + 1, sizeof(char));
    if (s == NULL)
        abort();
    memcpy(s, &len, sizeof(uint32_t));
    s += sizeof(uint32_t);
    if (ptr && len)
        memcpy(s, ptr, len);
    return s;
}

str str_new(const char *ptr)
{
    size_t len = ptr ? strlen(ptr) : 0;
    return str_newlen(ptr, len);
}

size_t str_len(str s)
{
    if (s == NULL)
        return 0;
    size_t len = 0;
    s -= sizeof(uint32_t);
    memcpy(&len, s, sizeof(uint32_t));
    return len;
}

void str_done(str s)
{
    if (s == NULL)
        return;
    s -= sizeof(uint32_t);
    free(s);
}

int main(void)
{
    str ok = str_new("hello world");
    if (ok == NULL) // fuck you gcc for false-positive compile error
        return 1;

    {
        size_t len = str_len(ok);
        printf("%s:%zu\n", ok, len);
        str_done(ok);
    }

    str q = str_new("0ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ");
    if (q == NULL)
        return 1;

    size_t len = str_len(q);
    char c = q[0];
    printf("%s:%zu:%c\n", q, len, c);
    str_done(q);
}
