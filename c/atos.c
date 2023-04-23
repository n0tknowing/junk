#include <ctype.h>

short atos(const char *nptr)
{
    short sign, result;
    const char *temp;

    if (!nptr || !*nptr)
        return 0;

    sign = 1;
    result = 0;
    temp = nptr;

    if (temp[0] == '-') {
        sign = -1;
        temp++;
    }

    while (*temp) {
        if (isdigit(*temp)) {
            result *= 10;
            result += (*temp - '0') * sign;
        }
        temp++;
    }

    return result;
}
