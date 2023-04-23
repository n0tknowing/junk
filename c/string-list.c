#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct str {
	char *data;
	size_t length;
	size_t count;
	struct str *next;
};

enum method {
	ASCENDING = 0,
	DESCENDING
};

static struct str *str_init(void)
{
	struct str *new = malloc(sizeof(struct str));
	if (!new)
		return NULL;

	new->data = NULL;
	new->length = 0;
	new->count = 0;
	new->next = NULL;

	return new;
}

static int str_push(struct str **this, char *data)
{
	struct str *new = str_init();
	if (!new)
		return -1;

	(*this)->count += 1;

	new->data = strdup(data);
	new->next = *this;
	new->length = strlen(data);
	new->count = (*this)->count;

	*this = new;
	return 0;
}

static int str_pop(struct str **this)
{
	struct str *t;
	if (!*this)
		return -1;

	(*this)->next->count -= 1;
	t = *this;
	*this = t->next;

	free(t->data);
	free(t);

	return 0;
}

static void str_destroy(struct str **this)
{
	struct str *p = *this;
	while (p) {
		struct str *t = p->next;
		free(p->data);
		free(p);
		p = t;
	}

	*this = NULL;
}

static size_t str_get_size(struct str *this)
{
	return this->count;
}

static size_t str_get_length(struct str *this)
{
	return this->length;
}

static int str_replace_at(struct str *this, size_t idx, const char *new)
{
	if (!this || !(this->data) || !new || idx > str_get_size(this) - 1)
		return -1;

	size_t what = str_get_size(this) - idx - 1;
	for (size_t i = 0; this; i++) {
		struct str *t = this->next;
		if (i == what)
			break;
		this = t;
	}

	free(this->data);
	this->data = strdup(new);
	this->length = strlen(new);
	if (!(this->data))
		return -1;

	return 0;
}

static char *str_get_at(struct str *this, size_t idx)
{
	if (!this || !(this->data) || idx > str_get_size(this) - 1)
		return NULL;

	size_t what = str_get_size(this) - idx - 1;
	for (size_t i = 0; this; i++) {
		struct str *t = this->next;
		if (i == what)
			return this->data;
		this = t;
	}

	return NULL;
}

static void str_print_asc(struct str *this)
{
	size_t l = str_get_size(this);
	for (size_t i = 0; i < l; i++) {
		const char *k = str_get_at(this, i);
		if (k)
			printf("%s\n", k);
	}

}

static void str_print_dsc(struct str *this)
{
	while (this) {
		struct str *t = this->next;
		if (this->data)
			printf("%s\n", this->data);
		this = t;
	}
}

static void str_print_all(struct str *this, enum method m)
{
	switch (m) {
	case DESCENDING:
		str_print_dsc(this);
		break;
	case ASCENDING: default:
		str_print_asc(this);
		break;
	}
}

static char **str_iter(struct str *this)
{
	if (!this)
		return NULL;

	char **s = calloc(str_get_size(this), sizeof(void *));
	if (!s)
		return NULL;

	size_t i = 0;
	while (this && this->data) {
		struct str *t = this->next;
		s[i] = strdup(this->data);
		this = t;
		i += 1;
	}

	return s;
}

static void str_free_iter(struct str *this, char **iter)
{
	size_t l = str_get_size(this);
	for (size_t i = 0; i < l; i++)
		free(iter[i]);

	free(iter);
}

int main(void)
{
	struct str *s = str_init();
	if (!s) {
		fprintf(stderr, "failed to init string\n");
		return 1;
	}

	for (int i = 1; i <= 10; i++) {
		char buf[10] = {0};
		snprintf(buf, 10, "str-%d", i);
		str_push(&s, buf);
		printf("length = %zu\n", str_get_length(s));
	}

	putchar('\n');

//	str_print_all(s, DESCENDING); putchar('\n');
/*	if (str_replace_at(s, 2, "test") < 0)
		fprintf(stderr, "failed\n");
	str_print_all(s, ASCENDING);
*/
	{
		char **test = str_iter(s);
		if (test) {
			for (size_t i = 0u; i < str_get_size(s); i++)
				printf("%s\n", test[i]);
			str_free_iter(s, test);
		}
	}

	str_destroy(&s);
	return 0;
}
