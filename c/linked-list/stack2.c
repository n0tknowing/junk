#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list {
	struct list *next;
	int data;
};

struct stack {
	struct list *head;
};

/* function for creating new stack, initialize head to 0 (NULL) */
/* fungsi untuk membuat stack baru, inisialisasi head ke 0 (NULL) */
static struct stack *s_new(void)
{
	struct stack *s = malloc(sizeof(struct stack));
	s->head = NULL;

	return s;
}

/* push a new data just requires allocating another struct */
/* push sebuah data baru cukup alokasi struct lainnya */
static void s_push(struct stack *s, int data)
{
	/* alokasi struct baru */
	struct list *l = malloc(sizeof(struct list));
	if (!l) {
		fprintf(stderr, "malloc: %s\n", strerror(errno));
		return;
	}

	l->data = data;  /* push data ke struct baru */
	l->next = s->head;  /* point ke head dari pointer baru */
	s->head = l;  /* sekarang, head pointer lama punya pointer baru */
}

static int s_not_empty(struct stack *s)
{
	return s->head != NULL;
}

/* removing a data just requires moving the pointer to the first
 * element to point to the second element instead, and then
 * freeing the first element. */
static int s_pop(struct stack *s)
{
	int ret;
	struct list *l;

	if (!s_not_empty(s)) {
		fprintf(stderr, "stack empty\n");
		return 0;
	}

	/* save old value that'll use as top of stack value */
	ret = s->head->data;

	/* patch out first value */
	l = s->head;  /* l now hold old pointer */
	s->head = l->next;  /* point to next pointer */

	free(l);  /* freeing old */

	return ret;
}

static void s_print(struct stack *s)
{
	struct list *l;

	for (l = s->head; l; l = l->next)
		printf("%d ", l->data);

	printf("\n");
}

static int s_top(struct stack *s)
{
	return s->head ? s->head->data : -1;  /* -1 mean stack empty */
}

static int s_len(struct stack *s)
{
	int len = 0;  /* for counting */
	struct list *l;

	for (l = s->head; l; l = l->next)
		len++;

	return len;
}

/* clearing stack almost same as destroying stack
 * except we are not freeing the stack itself, but only
 * popped all pushed data.
 * or more efficient, without looping the stack,
 * just assigning an empty stack to old stack. */
static void s_destroy(struct stack *s)
{
	/* because we want destroy the stack, it mean
	 * all pushed data should be popped first before
	 * we destroying the stack itself */
	while (s_not_empty(s))
		s_pop(s);

	free(s);
}

int main(void)
{
	int i;
	struct stack *s = s_new();

	for (i = 0; i < 11; i++) {
		s_push(s, i);
		s_print(s);
	}

	printf("top now: %d\n", s_top(s));
	printf("length stack now: %d\n", s_len(s));

	while (s_not_empty(s)) {
		s_pop(s);
		s_print(s);
	}

	printf("%d\n", s_top(s));

	s_destroy(s);

	return EXIT_SUCCESS;
}
