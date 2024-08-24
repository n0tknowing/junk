#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct block {
    struct block *next;
    uint32_t count, capacity;
    void *data;
};

struct allocator {
    struct block *head;
    struct block *curr; /* current block in use */
    size_t data_size;
};

static struct block *block_new(size_t capacity, size_t size)
{
    struct block *block;

    block = calloc(1, sizeof(struct block));
    assert(block != NULL);

    block->count = 0;
    block->capacity = (uint32_t)capacity;
    block->data = calloc(capacity, size);
    assert(block->data != NULL);

    return block;
}

void allocator_setup(struct allocator *a, size_t capa, size_t size)
{
    struct block *block;

    block = block_new(capa, size);
    a->data_size = size;
    a->head = a->curr = block;
}

void allocator_cleanup(struct allocator *a)
{
    struct block *head, *next;

    head = a->head;
    while (head != NULL) {
        next = head->next;
        free(head->data);
        free(head);
        head = next;
    }

    a->data_size = 0;
    a->head = a->curr = NULL;
}

void allocator_reset(struct allocator *a)
{
    struct block *head, *next;

    head = a->head;
    while (head != NULL) {
        next = head->next;
        if (head->count > 0) {
            memset(head->data, 0, head->count * a->data_size);
            head->count = 0;
        }
        head = next;
    }

    a->curr = a->head;
}

void *allocator_malloc(struct allocator *a)
{
    void *ptr;
    struct block *curr, *nblk;

    curr = a->curr;
    if (curr->count == curr->capacity) {
        if (curr->next != NULL) {
            a->curr = curr->next;
            curr = curr->next;
        } else {
            nblk = block_new(curr->capacity, a->data_size);
            nblk->next = NULL;
            curr->next = nblk;
            a->curr = nblk;
            curr = nblk;
        }
    }

    ptr = (unsigned char *)curr->data + curr->count * a->data_size;
    curr->count++;
    return ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct node {
    int v;
    struct node *next;
};

int main(void)
{
    struct allocator alloc;
    allocator_setup(&alloc, 8, sizeof(struct node));

    struct node *head = allocator_malloc(&alloc);
    head->v = 1;
    head->next = NULL;

    for (int i = 2; i <= 4096; i++) {
        struct node *temp = allocator_malloc(&alloc);
        temp->v = i;
        temp->next = head;
        head = temp;
    }

    struct node *next;
    while (head != NULL) {
        next = head->next;
        head = next;
    }

    allocator_reset(&alloc);

    head = allocator_malloc(&alloc);
    head->v = 1;
    head->next = NULL;

    for (int i = 2; i <= 1024; i++) {
        struct node *temp = allocator_malloc(&alloc);
        temp->v = i;
        temp->next = head;
        head = temp;
    }

    while (head != NULL) {
        next = head->next;
        head = next;
    }

    allocator_cleanup(&alloc);
}
