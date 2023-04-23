// compile and run with:
//   $ gcc -Wall -Wextra -std=c11 -fsanitize=thread,undefined -lpthread thr.c
//   $ ./a.out
#define _GNU_SOURCE
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct worker {
    int id;
    struct timespec tm;
};

static void *work(void *arg)
{
    struct worker *r = arg;
    const struct timespec *t = (const struct timespec *)&r->tm;
    printf(" Worker %02d sleeps for %ld seconds\n", r->id, (long)t->tv_sec);
    (void)nanosleep(t, NULL);
    printf("  Worker %02d wake up!\n", r->id);
    return NULL;
}

// feel free to change this
#define NR_THREADS  4

int main(void)
{
    pthread_t thr[NR_THREADS];

    // put each worker struct in heap since every thread has its own
    // stack but shared heap.
    // calloc() is used over malloc() to avoid memset() call to zero
    // the new allocated memory.
    struct worker *wrk = calloc(NR_THREADS, sizeof(*wrk));
    if (wrk == NULL)
        return 1;

    srand(time(NULL));
    for (int i = 0; i < NR_THREADS; i++) {
        wrk[i].id = i + 1;
        wrk[i].tm.tv_sec = rand() % 15;
        int rc = pthread_create(&thr[i], NULL, work, &wrk[i]);
        if (rc) {
            printf("pthread_create(): %s\n", strerror(rc));
            continue;
        }
    }

    for (int i = 0; i < NR_THREADS; i++) {
        int rc = pthread_join(thr[i], NULL);
        if (rc)
            printf("pthread_join(): %s\n", strerror(rc));
    }

    free(wrk);
}
