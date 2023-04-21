#include <stdlib.h>

#include "slist.h"

struct slist *slist_init(void)
{
	struct slist *this = malloc(sizeof(struct slist));
	if (!this)
		return NULL;

	this->head = NULL;
	this->elems = 0;

	return this;
}

int slist_find(struct slist *this, int v)
{
	struct node *head = slist_get_head(this);
	
	while (head) {
		if (head->data == v)
			return head->data;
		head = slist_get_next(head);
	}

	return -1;
}

int slist_insert(struct slist *this, int data)
{
	struct node *n = malloc(sizeof(struct node));
	if (!n)
		return -1;

	n->data = data;
	n->next = slist_get_head(this);
	this->head = n;
	this->elems++;

	return 0;
}

void slist_delete(struct slist *this)
{
	struct node *tmp;

	if (slist_is_empty(this))
		return;

	tmp = slist_get_head(this);
	this->head = slist_get_next(tmp);

	this->elems--;
	free(tmp);
}

void slist_clear(struct slist *this)
{
	while (!slist_is_empty(this))
		slist_delete(this);
}

void slist_end(struct slist *this)
{
	if (this) {
		slist_clear(this);
		free(this);
	}
}
