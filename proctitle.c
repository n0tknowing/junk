/* some hack to change process title on ps/top/htop */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* strdup, memset */
#include <unistd.h>  /* getopt, sleep to prevent excessive CPU usage */

int main(int argc, char **argv)
{
    int rc;
    char *user, *pass;
    const char *prog = argv[0];

    user = pass = NULL;

    while ((rc = getopt(argc, argv, "u:p:")) != -1) {
        switch (rc) {
        case 'u':
            user = strdup(optarg);
            memset(optarg, 0, strlen(optarg));
            break;
        case 'p':
            pass = strdup(optarg);
            memset(optarg, 0, strlen(optarg));
            break;
        default:
            fprintf(stderr, "usage: %s -u USER -p PASS\n", prog);
            return 1;
        }
    }

    /* idk why i wrote these */
    argc -= optind;
    argv += optind;

    if (!user || !pass) {
        fprintf(stderr, "usage: %s -u USER -p PASS\n", prog);
        return 1;
    }

    int i;
    for (i = 0; i < 100; i++) {
        sleep(1);
        if (i == 99) {
            printf("Username: %s\n", user?user:"none");
            printf("Password: %s\n", pass?pass:"none");
        }
    }

    if (pass)
        free(pass);
    if (user)
        free(user);

    return 0;
}
