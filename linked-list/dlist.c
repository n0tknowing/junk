#include <stdlib.h>

#include "dlist.h"

struct list *dlist_init(void)
{
    struct list *this = malloc(sizeof(struct list));
    if (!this)
        return NULL;

    this->head = NULL;
    this->tail = NULL;
    this->elems = 0;

    return this;
}

int dlist_find(struct list *this, int v)
{
    struct node *head = dlist_get_head(this);
    struct node *tail = dlist_get_tail(this);

    while (head != tail) {
        if (head->data == v)
            return head->data;
        if (tail->data == v)
            return tail->data;
        head = dlist_get_next(head);
        tail = dlist_get_prev(tail);
    }

    return -1;
}

int dlist_insert_head(struct list *this, int data)
{
    struct node *n = malloc(sizeof(struct node));
    if (!n)
        return -1;

    n->data = data;
    n->prev = NULL;
    n->next = dlist_get_head(this);

    /* if list is empty, insert data to the tail */
    if (dlist_is_empty(this))
        this->tail = n;
    else /* otherwise, insert data to the previous head node */
        this->head->prev = n;

    this->elems++;
    this->head = n;
    return 0;
}

int dlist_insert_tail(struct list *this, int data)
{
    struct node *n = malloc(sizeof(struct node));
    if (!n)
        return -1;

    n->data = data;
    n->next = NULL;
    n->prev = dlist_get_tail(this);

    /* if list is empty, insert data to the head */
    if (dlist_is_empty(this))
        this->head = n;
    else /* otherwise, insert data to the next tail node */
        this->tail->next = n;

    this->elems++;
    this->tail = n;
    return 0;
}

void dlist_delete_head(struct list *this)
{
    struct node *tmp;

    if (!this || dlist_is_empty(this))
        return;

    tmp = this->head;

    if (dlist_get_size(this) == 1) {
        this->head = NULL;
        this->tail = NULL;
    } else {
        this->head = dlist_get_next(tmp);
        this->head->prev = NULL;
    }

    this->elems--;
    free(tmp);
}

void dlist_delete_tail(struct list *this)
{
    struct node *tmp;

    if (!this || dlist_is_empty(this))
        return;

    tmp = this->tail;

    if (dlist_get_size(this) == 1) {
        this->head = NULL;
        this->tail = NULL;
    } else {
        this->tail = dlist_get_prev(tmp);
        this->tail->next = NULL;
    }

    this->elems--;
    free(tmp);
}

void dlist_clear(struct list *this)
{
    while (!dlist_is_empty(this))
        dlist_delete_head(this);
}

void dlist_end(struct list *this)
{
    if (this) {
        dlist_clear(this);
        free(this);
    }
}
