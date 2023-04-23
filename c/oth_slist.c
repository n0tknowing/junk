/*
 * Singly linked list.
 */

#include <stdio.h>
#include <stdlib.h>

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

struct slist {
    struct slist *next;
    void *data;
};

struct slist *slist_last(struct slist *head)
{
    struct slist *tail = head;
    if (tail == NULL)
        return head;

    while (tail->next != NULL)
        tail = tail->next;

    return tail;
}

struct slist *slist_prepend(struct slist *head, void *data)
{
    struct slist *next = malloc(sizeof(struct slist));
    if (unlikely(next == NULL))
        return head;  /* Don't clobber old element. */

    next->next = head;
    next->data = data;

    return next;
}

/* Note that this is O(N) insertion since it needs to
 * traverse list to get last element.
 */
struct slist *slist_append(struct slist *head, void *data)
{
    struct slist *last;
    struct slist *next = malloc(sizeof(struct slist));
    if (unlikely(next == NULL))
        return head;  /* Don't clobber old element. */

    next->next = NULL;
    next->data = data;

    if (head == NULL) /* Empty list, return */
        return next;

    last = slist_last(head);
    last->next = next;

    return head;
}

/* Another approach is add new element to 'struct slist' to
 * track its length, so we no need to traverse list, but
 * its downside is the size of 'struct slist' become bigger,
 * maybe this is not you want if you really *care* about memory
 * usage.
 */
size_t slist_length(struct slist *head)
{
    size_t count = 0;
    struct slist *temp = head;

    while (temp != NULL) {
        count += 1;
        temp = temp->next;
    }

    return count;
}

void slist_destroy(struct slist *head)
{
    while (head != NULL) {
        struct slist *next = head->next;
        free(head);
        head = next;
    }
}

int main(void)
{
    struct slist *head = NULL, *tail;
    int a[6] = {1, 2, 3, 4, 5, 6};

    head = slist_append(head, &a[0]);
    head = slist_append(head, &a[1]);
    head = slist_append(head, &a[2]);

    printf("Count = %zu\n", slist_length(head));
    printf("Head  = %d\n", *((int *)head->data));

    tail = slist_last(head);
    printf("Tail  = %d\n", *((int *)tail->data));

    head = slist_prepend(head, &a[3]);
    head = slist_prepend(head, &a[4]);
    head = slist_prepend(head, &a[5]);

    printf("Count = %zu\n", slist_length(head));
    printf("Head  = %d\n", *((int *)head->data));

    tail = slist_last(head);
    printf("Tail  = %d\n", *((int *)tail->data));

    slist_destroy(head);
    return 0;
}
