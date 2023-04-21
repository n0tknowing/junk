#include <err.h>
#include <stdio.h>

#include "slist.h"

static void print(struct slist *this)
{
	struct node *head = slist_get_head(this);

	while (head) {
		printf("%d -> ", head->data);
		head = slist_get_next(head);
	}

	printf("NULL\n");
}

int main(void)
{
	struct slist *list = slist_init();
	if (!list)
		err(1, "slist_init");

	int ret = 0;

	print(list);
	for (int i = 10; i >= 1; i--) {
		if (slist_insert(list, i) < 0) {
			ret = 1;
			goto err;
		}
		print(list);
	}

	size_t size = slist_get_size(list);
	for (size_t i = 0; i < size; i++) {
		slist_delete(list);
		print(list);
	}

err:
	if (ret)
		fprintf(stderr, "slist_insert failed\n");
	slist_end(list);
	return ret;
}
