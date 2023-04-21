#ifndef DOUBLY_H
#define DOUBLY_H

#include <stddef.h>

struct node {
	int data;
	struct node *next;
	struct node *prev;
};

struct list {
	struct node *head;
	struct node *tail;
	size_t elems;
};

#define dlist_get_size(__list)     (__list->elems)
#define dlist_is_empty(__list)     (dlist_get_size(__list) == 0)

#define dlist_get_head(__list)     (__list->head)
#define dlist_get_tail(__list)     (__list->tail)
#define dlist_get_next(__node)     (__node->next)
#define dlist_get_prev(__node)     (__node->prev)

struct list *dlist_init(void);

int dlist_find(struct list *, int);
int dlist_insert_head(struct list *, int);
int dlist_insert_tail(struct list *, int);
void dlist_delete_head(struct list *);
void dlist_delete_tail(struct list *);
void dlist_clear(struct list *);
void dlist_end(struct list *);

#endif
