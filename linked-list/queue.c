#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* dequeue = pop an item
 * enqueue = push an item */

struct list {
    struct list *next;
    int value;
};

struct queue {
    struct list *head;  /* dequeue this next */
    struct list *tail;  /* enqueue after this */
};

static struct queue *q_new(void)
{
    struct queue *q;

    q = malloc(sizeof(struct queue));
    if (!q)
        return NULL;

    q->head = NULL;
    q->tail = NULL;

    return q;
}

static void q_enq(struct queue *q, int value)
{
    struct list *l;

    l = malloc(sizeof(struct list));
    if (!l) {
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        return;
    }

    l->value = value;
    l->next = NULL;

    if (!q->head) {
        q->head = l;
    } else {
        q->tail->next = l;
    }

    q->tail = l;
}

static int q_not_empty(const struct queue *q)
{
    return q->head != NULL;
}

static int q_deq(struct queue *q)
{
    int ret;
    struct list *l;

    if (!q_not_empty(q)) {  /* empty */
        return 0;
    }

    ret = q->head->value;
    l = q->head;
    q->head = l->next;

    free(l);

    return ret;
}

static void q_print(struct queue *q)
{
    struct list *l;

    for (l = q->head; l; l = l->next) {
        printf("%d ", l->value);
    }

    printf("\n");
}

static void q_destroy(struct queue *q)
{
    while (q_not_empty(q)) {  /* while not empty */
        q_deq(q);
    }

    free(q);
}

int main(void)
{
    int i;
    struct queue *q;

    q = q_new();

    for (i = 0; i < 11; i++) {
        q_enq(q, i);
        q_print(q);
    }

    while (q_not_empty(q)) {  /* while not empty */
        q_deq(q);
        q_print(q);
    }

    q_destroy(q);

    return 0;
}
