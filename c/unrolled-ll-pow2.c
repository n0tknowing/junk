#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define unlikely(x)   (__builtin_expect(!!(x), 0))
#define __nodiscard    __attribute__((warn_unused_result))

#ifndef N
#define N 1000000
#endif

#ifndef START
#define START   8
#endif

struct val {
    uint64_t k;
    uint64_t v;
};

struct ulist {
    struct val *value;
    struct ulist *next;
    struct ulist *prev;
    size_t count;
    size_t cap;
};

static inline void ulist__add(struct ulist *el,
                              struct ulist *prev, struct ulist *next)
{
    prev->next = el;
    el->prev = prev;
    el->next = next;
    next->prev = el;
}

__nodiscard
struct ulist *ulist_add(struct ulist *head, const struct val v)
{
    struct ulist *curr;
    struct ulist *tmp;
    size_t alsz, newcap;

    if (head == NULL) {
        alsz = sizeof(struct ulist) + START * sizeof(struct val);
        head = calloc(1, alsz);
        if (unlikely(head == NULL))
            abort(); // will change in the future
        head->value = (struct val *)((uint8_t *)head + sizeof(struct ulist));
        head->cap = START;
        head->next = head;
        head->prev = head;
        curr = head;
    } else if (curr = head->prev, curr->count == curr->cap) {
        newcap = curr->cap * 2;
        printf("newcap = %zu\n", newcap);
        alsz = sizeof(struct ulist) + newcap * sizeof(struct val);
        tmp = calloc(1, alsz);
        if (unlikely(tmp == NULL))
            abort(); // will change in the future
        ulist__add(tmp, head->prev, head);
        curr = tmp;
        curr->cap = newcap;
        curr->value = (struct val *)((uint8_t *)curr + sizeof(struct ulist));
    }

    curr->value[curr->count++] = v;
    return head;
}

size_t ulist_size(struct ulist *head)
{
    size_t size = 0;
    struct ulist *temp = head->next;
    while (temp != head) {
        size += temp->count;
        temp = temp->next;
    }
    return size + head->count;
}

void ulist_clear(struct ulist *head)
{
    struct ulist *temp = head->next;
    while (temp != head) {
        struct ulist *next = temp->next;
        free(temp);
        temp = next;
    }
    free(head);
}

int main(void)
{
    struct ulist *head = NULL;

    clock_t begin = clock();
    for (uint64_t i = 0; i < N; i++)
        head = ulist_add(head, (const struct val){i, i + 1});
    clock_t end = clock();
    double elapsed_secs = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%d ops in %.3f secs, %.0f ns/op, %.0f op/sec\n",
            N, elapsed_secs, elapsed_secs / (double)N * 1e9,
            (double)N / elapsed_secs);
    ulist_clear(head);
}
