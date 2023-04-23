/* print other process environment using given PID */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s PID\n", argv[0]);
        return 1;
    }

    int pid, c;
    char env_path[128] = {0};
    FILE *fp;

    if (!strcmp(argv[1], "self")) {
        pid = getpid();
    } else {
        pid = atoi(argv[1]);
        if (pid < 0) {
            fprintf(stderr, "invalid PID\n");
            return 1;
        }
    }

    snprintf(env_path, 127, "/proc/%d/environ", pid);
    fp = fopen(env_path, "r");
    if (!fp) {
        fprintf(stderr, "%s: %s\n", env_path, strerror(errno));
        return 1;
    }

    while ((c = fgetc(fp)) != EOF)
        putchar(c ? c : '\n');

    fclose(fp);
    return 0;
}
