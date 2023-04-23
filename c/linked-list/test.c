#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"

static void print_from_head(struct list *this)
{
	int data;
	struct node *temp = dlist_get_head(this);

	printf("NULL -> ");

	while (temp) {
		data = temp->data;
		printf("%d -> ", data);
		temp = dlist_get_next(temp);
	}

	printf("NULL\n");
}

int main(void)
{
	int i, ret;
	struct list *new;

	new = dlist_init();
	if (new == NULL) {
		fprintf(stderr, "dlist_init failed\n");
		return 1;
	}

	ret = 0;

	printf("Is empty? = %d\n", dlist_is_empty(new));
	printf("Size before = %zu\n\n", dlist_get_size(new));

	print_from_head(new);
	for (i = 250000; i >= 1; i--) {
		if (dlist_insert_tail(new, i) < 0) {
			ret = 1;
			goto end;
		}
//		print_from_head(new);
	}

#if 0
	size_t s = dlist_get_size(new);
	for (size_t j = 0; j < s; j++) {
		dlist_delete_tail(new);
		print_from_head(new);
	}
#endif

	i = dlist_find(new, 125000);
	if (i != -1)
		printf("OK\n");

end:
	if (ret)
		fprintf(stderr, "error\n");
	dlist_end(new);
	return ret;
}
