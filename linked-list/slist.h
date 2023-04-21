#ifndef slist_h
#define slist_h

#include <stddef.h>

struct node {
	int data;
	struct node *next;
};

struct slist {
	struct node *head;
	size_t elems;
};

#define slist_get_size(__list)     (__list->elems)
#define slist_is_empty(__list)     (slist_get_size(__list) == 0)

#define slist_get_head(__list)     (__list->head)
#define slist_get_next(__node)     (__node->next)

struct slist *slist_init(void);

int slist_find(struct slist *, int);
int slist_insert(struct slist *, int);
void slist_delete(struct slist *);
void slist_clear(struct slist *);
void slist_end(struct slist *);

#endif
