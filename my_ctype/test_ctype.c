/**
 * From https://github.com/postgres/postgres/blob/master/src/test/locale/test-ctype.c
 *
 *    test-ctype.c
 *
 *    Written by Oleg BroytMann, phd2@earthling.net
 *       with help from Oleg Bartunov, oleg@sai.msu.su
 *    Copyright (C) 1998 PhiloSoft Design
 *
 *    This is copyrighted but free software. You can use it, modify and distribute
 *    in original or modified form providing that the author's names and the above
 *    copyright notice will remain.
 *
 *    Disclaimer, legal notice and absence of warranty.
 *       This software provided "as is" without any kind of warranty. In no event
 *       the author shall be liable for any damage, etc.
 */

#include "ctype.h"
#include <stdio.h>

static char *flag(int c)
{
    return c ? "yes" : "no";
}

static void describe_char(int c)
{
    unsigned char cp = (unsigned char)c,
                  up = to_upper(c),
                  lo = to_lower(c);
 
    if (!is_print(cp))
        cp = ' ';

    if (!is_print(up))
        up = ' ';

    if (!is_print(lo))
        lo = ' ';
 
    printf("chr#%-4d%2c%6s%6s%6s%6s%6s%6s%6s%6s%6s%6s%6s%6s%4c%4c\n",
            c, cp,
            flag(is_alnum(c)), flag(is_alpha(c)), flag(is_cntrl(c)),
            flag(is_digit(c)), flag(is_lower(c)), flag(is_graph(c)),
            flag(is_print(c)), flag(is_punct(c)), flag(is_space(c)),
            flag(is_upper(c)), flag(is_xdigit(c)), flag(is_blank(c)),
            up, lo);
}

int main(void)
{
    printf("char#  char alnum alpha cntrl digit lower graph print punct space upper xdigit blank lo up\n");
    for (int c = 0; c < 256; c++)
        describe_char(c);
}
