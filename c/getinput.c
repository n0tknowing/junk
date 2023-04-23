#define _GNU_SOURCE
#include <stdio.h>

#define unlikely(x) (__builtin_expect(!!(x), 0))

/**
 * same as fgets(), except it handles input without newline and splits input
 * more than `n` with "\n\0" included.
 *
 * for example, if we have:
 *
 *     const char *input = "hello world\n\0"
 *     char buffer[4] = {0};
 *
 * it will splits the input into:
 *
 *    "he\n\0" -> "he" (ret=3)
 *    "ll\n\0" -> "ll" (ret=3)
 *    "o \n\0" -> "o " (ret=3)
 *    "wo\n\0" -> "wo" (ret=3)
 *    "rl\n\0" -> "rl" (ret=3)
 *    "d\n\0"  -> "d"  (ret=2)
 *
 * in case your input is really splitted, keep calling the function until
 * it returns less than `n`.
 *
 * on success, it returns the number of character read including newline ('\n')
 * but not including null byte ('\0').
 * otherwise returns 0.
 */
static size_t getinput(char *in, size_t n, FILE *fp)
{
    int ch;
    unsigned char *p;

    if (unlikely(in == NULL) || unlikely(fp == NULL)) {
        return 0;
    } else if (unlikely(n == 0)) {
        return 0;
    } else if (unlikely(n == 1)) {
        *in = '\0';
        return 0;
    }

    p = (unsigned char *)in;
    n -= 1; /* for '\0' */

    /**
     * note that character encoding other than ASCII is accepted here.
     * so make sure your buffer large enough to handles it otherwise it will
     * splitted and you will get garbage result.
     */
    while (n > 1 && (ch = getc_unlocked(fp)) != EOF && ch != '\0') {
        *p++ = (unsigned char)ch; /* integer promotions are annoying */
        if (ch == '\n')
            break;
        n--;
    }

    if (n == 1) /* truncated or empty input or maybe done */
        *p++ = '\n';

    *p = '\0';
    return (size_t)(p - (unsigned char *)in); /* always > 0 */
}

int main(void)
{
    char in[64];
    size_t nread;

    while ((nread = getinput(in, sizeof(in), stdin)) != 0) {
        printf("length = %zu\n", nread);
        printf("data   = %s", in);
    }
}
