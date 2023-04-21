/* dynamic array for string
 * ------------------------
 * restrictions:
 * - cannot delete only one entry, so you need to delete all entries instead.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>

#define ARR_MAX_SIZE	4096
#define MAX(x, y)	(((x) > (y)) ? (x) : (y))
#define MIN(x, y)	(((x) < (y)) ? (x) : (y))

struct str_list {
	char **data;
	size_t capacity;
	size_t entry;
};

struct str_list *str_init(const size_t init_cap)
{
	struct str_list *this = malloc(sizeof(struct str_list));
	if (!this)
		return NULL;

	this->entry = 0;
	this->capacity = (init_cap == 0 || init_cap > ARR_MAX_SIZE) ? 25 : init_cap;
	this->data = calloc(this->capacity, sizeof(char *));
	if (!this->data) {
		free(this);
		return NULL;
	}

	return this;
}

int str_grow(struct str_list *this, size_t new_cap)
{
	if (this && new_cap > this->capacity) {
		new_cap = MIN(new_cap, ARR_MAX_SIZE);
		char **tmp = realloc(this->data, new_cap * sizeof(char *));
		if (!tmp)
			return -1;
		this->data = tmp;
		size_t offset = sizeof(char *) * (new_cap - this->capacity);
		memset(this->data + this->capacity, 0, offset);
		this->capacity = new_cap;
		return 0;
	}

	return -1;
}

int str_insert(struct str_list *this, const char *data)
{
	if (this && data) {
		if (this->entry == this->capacity) {
			if (str_grow(this, this->capacity * 2) < 0)
				return -1;
		}
		char *new = strdup(data);
		this->data[this->entry] = new ? new : NULL;
		this->entry++;
		return 0;
	}

	return -1;
}

void str_clear(struct str_list *this)
{
	if (this) {
		if (this->entry > 0) {
			for (size_t i = 0; i < this->entry; i++) {
				free(this->data[i]);
				this->data[i] = NULL;
			}
		}
		free(this->data);
		this->data = NULL;
	}
}

void str_end(struct str_list *this)
{
	if (this) {
		str_clear(this);
		free(this);
	}
}

#if DO_TEST
#include <assert.h>
#include <stdio.h>

#define success_assert(x) (assert((x) == 0))
#define failed_assert(x) (assert((x) == -1))

static void print(struct str_list *this)
{
	for (size_t i = 0; i < this->entry; i++)
		printf("[%zu] %s\n", i + 1, this->data[i] ? this->data[i] : "null");
}

int main(void)
{
	struct str_list *list = str_init(5);
	if (!list)
		return 1;
	
	printf("Entries: %zu\nCapacity: %zu\n\n", list->entry, list->capacity);

	success_assert(str_insert(list, "SATU"));
	success_assert(str_insert(list, "DUA"));
	success_assert(str_insert(list, "TIGA"));
	success_assert(str_insert(list, "EMPAT"));
	success_assert(str_insert(list, "LIMA"));
	failed_assert(str_insert(list, NULL));
	success_assert(str_insert(list, "ENAM"));

	printf("Entries: %zu\nCapacity: %zu\n", list->entry, list->capacity);
	print(list);
	printf("Last entry: %s\n\n", list->data[list->entry - 1]);

	printf("Growing to 50\n");
	success_assert(str_grow(list, 50));
	printf("Entries: %zu\nCapacity: %zu\n", list->entry, list->capacity);
	print(list);
	printf("Last entry: %s\n\n", list->data[list->entry - 1]);

	printf("Growing to %d\n", ARR_MAX_SIZE + 1);
	success_assert(str_grow(list, ARR_MAX_SIZE + 1));
	printf("Entries: %zu\nCapacity: %zu\n", list->entry, list->capacity);

	str_end(list);
	return 0;
}
#endif
