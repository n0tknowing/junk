#include <ctype.h>
#include <stdio.h>

#define EXPR_ERR    (-1)

/* return the next character from stdin w/o consuming it. */
static int peekchar(void)
{
    int c;

    c = getchar();
    if (c != EOF)
        ungetc(c, stdin);  /* puts it back */

    return c;
}

/* read an integer written in decimal notation from stdin until the first
 * non-digit and return it. return 0 if there are no digits. */
static int read_num(void)
{
    int acc = 0;  /* accumulator: the number so far */
    int c;    /* next character */

    while ((c = getchar()) != EOF && isdigit(c)) {
        c = getchar();    /* consume it */
        acc *= 10;        /* shift previous digits over */
        acc += c - '0'; /* add decimal value of new digit */
    }

    return acc;
}

/* read an expression from stdin and return its value
 * returns EXPR_ERR on error.
 *
 * this also an example of a recursive descent parser. */
static int read_expr(void)
{
    int e1, e2, c, op;

    c = peekchar();

    if (c == '(') {
        c = getchar();

        e1 = read_expr();
        op = getchar();
        e2 = read_expr();

        c = getchar();

        if (c != ')')
            return EXPR_ERR;

        switch (op) {
        case '*':
            return e1*e2;
        case '+':
            return e1+e2;
        case '-':
            return e1-e2;
        default:
            return EXPR_ERR;
        }
    } else if (isdigit(c)) {
        return read_num();
    } else {
        return EXPR_ERR;
    }
}

int main(void)
{
    int e;

    while ((e = read_expr()) != EXPR_ERR)
        printf("%d\n", e);

    return 0;
}
