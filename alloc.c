#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#define ALIGN(x, y)	(((x) + ((y) - 1)) & (~((y) - 1)))
#define IS_POW2(x)	(((x) & ((x) - 1)) == 0)

//////////////////////////////////////////////////////////////////////////////

struct allocator_freelist {
	struct allocator_freelist *next;
};

struct allocator_blob {
	struct allocator_blob *next;
	size_t used;
};

struct allocator {
	struct allocator_blob *blob;
	struct allocator_freelist *freelist;
	size_t elem_size, blob_data_offset, blob_capacity;
};

static unsigned char *blob_data(const struct allocator *alc,
								struct allocator_blob *blob)
{
	return (unsigned char *)blob + alc->blob_data_offset;
}

static void allocator_setup(struct allocator *alc,
							size_t elem_size,
							size_t elem_align,
							size_t min_capacity)
{
	assert(elem_align >= alignof(struct allocator_freelist) && IS_POW2(elem_align));
	assert(elem_size >= sizeof(struct allocator_freelist));
	assert(min_capacity != 0);

	alc->elem_size = ALIGN(elem_size, elem_align);
	alc->blob_data_offset = ALIGN(sizeof(struct allocator_blob), elem_align);
	alc->blob_capacity = min_capacity;
	alc->blob = NULL;
	alc->freelist = NULL;
}

static void allocator_cleanup(struct allocator *alc)
{
	struct allocator_blob *blob = alc->blob;

	while (blob) {
		struct allocator_blob *next = blob->next;
		munmap(blob, alc->blob_data_offset + (alc->blob_capacity * alc->elem_size));
		blob = next;
	}

	alc->elem_size = 0;
	alc->blob_capacity = 0;
	alc->blob = NULL;
	alc->freelist = NULL;
}

static void *allocator_alloc(struct allocator *alc)
{
	if (alc->freelist) {
		struct allocator_freelist *p = alc->freelist;
		alc->freelist = p->next;
		return (void *)p;
	}

	struct allocator_blob *blob = alc->blob;
	if (blob == NULL || blob->used == alc->blob_capacity) {
		blob = mmap(NULL,
					alc->blob_data_offset + (alc->blob_capacity * alc->elem_size),
					PROT_READ | PROT_WRITE,
					MAP_PRIVATE | MAP_ANONYMOUS,
					-1,
					0);
		if (blob == MAP_FAILED)
			return NULL;
		blob->used = 0;
		blob->next = alc->blob;
		alc->blob = blob;
	}

	void *p = blob_data(alc, blob) + (blob->used * alc->elem_size);
	blob->used++;
	return p;
}

static void allocator_free(struct allocator *alc, void *p)
{
	if (p == NULL) return;
	struct allocator_freelist *fl = (struct allocator_freelist *)p;
	fl->next = alc->freelist;
	alc->freelist = fl;
}

//////////////////////////////////////////////////////////////////////////////

typedef size_t arena_mark;

struct arena {
	struct allocator *backing;
	unsigned char *base;
	size_t used, capacity;
};

static bool arena_setup(struct arena *arn, struct allocator *backing)
{
	arn->backing = backing;
	arn->base = allocator_alloc(backing);
	arn->used = 0;
	arn->capacity = backing->elem_size;
	return arn->base != NULL;
}

static void arena_reset(struct arena *arn)
{
	arn->used = 0;
}

static void *arena_alloc(struct arena *arn, size_t size, size_t align)
{
	assert(align != 0 && IS_POW2(align));

	size_t off = ALIGN(arn->used, align);
	if (off > arn->capacity || size > arn->capacity - off) return NULL;
	unsigned char *p = arn->base + off;
	arn->used = off + size;
	return (void *)p;
}

static arena_mark arena_save(struct arena *arn)
{
	return (arena_mark)arn->used;
}

static void arena_restore(struct arena *arn, arena_mark mark)
{
	assert(mark <= arn->used);
	arn->used = mark;
}

//////////////////////////////////////////////////////////////////////////////

int main(void)
{
	struct foo {
		long double a;
		unsigned int b;
		unsigned char c;
		unsigned char d;
	};

	{
		struct allocator foo_alc;

		srand((unsigned int)time(NULL));

		allocator_setup(&foo_alc, sizeof(struct foo), alignof(struct foo), 16);

		for (int i = 0; i < (1<<8); i++) {
			struct foo *f = allocator_alloc(&foo_alc);
			assert(f != NULL);
			f->a = 1.0;
			f->b = 2;
			f->c = 'c';
			f->d = 'd';
			int maybe_free = rand() % 2;
			if (maybe_free)
				allocator_free(&foo_alc, f);
		}

		allocator_cleanup(&foo_alc);
	}

	{
		struct allocator byte_alc;
		struct arena byte_arn;

		allocator_setup(&byte_alc, 8192, alignof(max_align_t), 1);
		assert(arena_setup(&byte_arn, &byte_alc));

		char *test = arena_alloc(&byte_arn, 5, alignof(char));
		memcpy(test, "helo\0", 5);
		puts(test);
		printf("%zu\n", byte_arn.used);

		long double *test2 = arena_alloc(&byte_arn, sizeof(long double), alignof(long double));
		*test2 = 1.0;
		assert(((uintptr_t)test2 % alignof(long double)) == 0);
		printf("%zu\n", byte_arn.used);

		arena_mark mark = arena_save(&byte_arn);
		struct foo *test3 = arena_alloc(&byte_arn, sizeof(*test3), alignof(*test3));
		assert(((uintptr_t)test3 % alignof(struct foo)) == 0);
		printf("%zu\n", byte_arn.used);
		arena_restore(&byte_arn, mark);
		printf("%zu\n", byte_arn.used);

		arena_reset(&byte_arn);
		allocator_cleanup(&byte_alc);
	}

	return 0;
}
