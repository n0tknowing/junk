#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE   71

static void pb(uint64_t val)
{
    int idx = 0;
    char buf[BUFF_SIZE + 1];
    const uint8_t *bytes = (const uint8_t *)&val;

    for (int i = 7; i >= 0; i--) { // for each byte
        for (int j = 7; j >= 0; j--) { // for each bit
            buf[idx] = (bytes[i] & (1 << j)) ? '1' : '0';
            idx++;
        }
        if (idx < BUFF_SIZE) {
            buf[idx] = ' ';
            idx++;
        }
    }

    buf[idx] = 0;
    printf("Base-2   = %s\nBase-8   = %lo\nBase-10  = %lu\nBase-16  = 0x%02lx\n", buf, val, val, val);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    int base = 10;
    const char *arg = argv[1];

    if (strlen(arg) > 3 && !strncmp(arg, "0x", 2))
        base = 16;

    uint64_t v = strtoul(arg, NULL, base);
    pb(v);
}
