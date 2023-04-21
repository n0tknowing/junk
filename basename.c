#include <stdio.h>

static char *pd_basename(const char *old)
{
    const char *s;

    for (s = old; *old; old++) {
        if (*old == '/')
            s = old + 1;
    }

    return (char *)s;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    const char *pngf = pd_basename(argv[1]);
    puts(pngf);
}
