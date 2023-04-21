/* check if all parens in a string is balanced */

#include <stdio.h>
#include <string.h>

#include "slist.h"

static int is_balanced(const char *s, size_t l)
{
    int ret = 0;
    struct slist *stack;

    stack = slist_init();
    if (!stack)
        return 0;

    for (size_t i = 0; i < l; i++) {
        if (s[i] == '(') {
            if (slist_insert(stack, s[i]) < 0)
                goto err;
        } else if (s[i] == ')') {
            if (slist_is_empty(stack))
                goto err;
            slist_delete(stack);
        }
    }

    ret = slist_is_empty(stack);
err:
    slist_end(stack);
    return ret;
}

int main(void)
{
    const char *str = "(((()()()()()())";
    int res = is_balanced(str, strlen(str));
    printf("%s is%s balanced\n", str, res ? "" : " not");
    return 0;
}
