#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list {
	struct list *next;
	int data;
};

/* fungsi untuk push data baru.
 * cara kerja:
 *  - pertama, alokasi dulu pointer sementara
 *  - kedua, masukan nilai dari parameter fungsi ke pointer sementara
 *  - ketiga, pointer lama menyimpan data dari pointer sementara */
static void s_push(struct list **s, int data)
{
	/* alokasi struct baru */
	struct list *l = malloc(sizeof(struct list));
	if (!l) {
		fprintf(stderr, "malloc: %s\n", strerror(errno));
		return;
	}

	l->data = data;  /* push data ke struct baru */
	l->next = *s;  /* struct baru point ke struct lama */
	*s = l;  /* sekarang, struct lama punya data dan struct baru */
}

static int s_not_empty(struct list **s)
{
	return *s != NULL;
}

/* menghapus sebuah data cukup memindahkan pointer pertama ke pointer kedua
 * dan free pointer pertama */
static int s_pop(struct list **s)
{
	int ret;
	struct list *l;

	if (!s_not_empty(s)) {
		fprintf(stderr, "stack empty\n");
		return 0;
	}

	/* save old value that'll use as top of stack value */
	ret = (*s)->data;

	/* patch out first value */
	l = *s;  /* l now hold old pointer */
	*s = l->next;  /* point to next pointer */

	free(l);  /* freeing old */

	return ret;
}

static void s_print(struct list **s)
{
	struct list *l;

	for (l = *s; l; l = l->next)
		printf("%d ", l->data);

	printf("\n");
}

static int s_top(struct list **s)
{
	if (s_not_empty(s))
		return (*s)->data;
	else
		return -1;
}

static int s_len(struct list **s)
{
	int len = 0;
	struct list *l;

	for (l = *s; l; l = l->next)
		len++;

	return len;
}

int main(void)
{
	int i;
	struct list *s = NULL;

	for (i = 0; i < 11; i++) {
		s_push(&s, i);
		s_print(&s);
	}

	printf("top now: %d\n", s_top(&s));
	printf("length stack now: %d\n", s_len(&s));

	while (s_not_empty(&s)) {
		s_pop(&s);
		s_print(&s);
	}

	printf("%d\n", s_top(&s));

	return 0;
}
