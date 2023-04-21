// This is comment
// and shouldn't be printed

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

static char *in; /* this is comment */
static const char /* wow? */ *cur;
/**/ static /* aaaaa */const/*bbbb*/char *end; //yoo;

static void putback(void)
{
    if (cur > in)
        cur -= 1;
}

/*
 * m
 * u
 * lti
 * line
 *
 * comments
 *
 * // nested comments?
 * // lol not allowed if use c-style comment
 * */

static int next(void)
{
    if (cur < end) {
        cur += 1; // prefer readability!
        return cur[-1];
    }
    return EOF;
}

static void skip_long_comment(void)
{
    int c;

    while ((c = next()) != EOF) {
        switch (c) {
        case '*':
            c = next();
            if (c == '/') {
                putchar(' ');
                return;
            }
            break;
        case '/':
            c = next();
            if (c == '*')
                errx(2, "nested C-style comment is not allowed");
            break;
        default:
            break;
        }
    }
}

static void skip_short_comment(void)
{
    int c;

    while ((c = next()) != EOF) {
        if (c == '\n') {
            putback();
            return;
        }
    }
}

static void scan(void)
{
    int c;

    while ((c = next()) != EOF) {
        switch (c) {
        case '/':
            c = next();
            switch (c) {
            case '/':
                skip_short_comment();
                continue;
            case '*':
                skip_long_comment();
                continue;
            default:
                putback();
                putback();
                putchar(next());
                break;
            }
            break;
        default:
            putchar(c);
            break;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    FILE *fp = fopen(argv[1], "r");
    if (!fp)
        err(2, "failed to open %s", argv[1]);

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);

    if (sz < 1)
        errx(2, "empty file");

    in = calloc((size_t)sz + 1, sizeof(*in));
    if (!in)
        err(2, "calloc");

    fread(in, sizeof(*in), (size_t)sz, fp);
    fclose(fp);

    cur = in;
    end = in + sz;

    scan();
    free(in);
}
