#include <assert.h>
#include <stdlib.h>

struct block {
    struct block *prev;
    size_t count;
    size_t capa;
    void *data;
};

struct allocator {
    struct block *head; /* current block */
    size_t block_data_size;
};

static struct block *block_new(size_t capa, size_t size)
{
    struct block *block;

    block = calloc(1, sizeof(struct block) + size * capa);
    assert(block != NULL);

    block->prev = NULL;
    block->count = 0;
    block->capa = capa;
    block->data = (unsigned char *)block + sizeof(struct block);
    return block;
}

void allocator_setup(struct allocator *a, size_t capa, size_t size)
{
    struct block *block;

    block = block_new(capa, size);
    a->head = block;
    a->block_data_size = size;
}

void allocator_cleanup(struct allocator *a)
{
    struct block *head, *prev;

    head = a->head;
    while (head != NULL) {
        prev = head->prev;
        free(head);
        head = prev;
    }

    a->block_data_size = 0;
}

void *allocator_malloc(struct allocator *a)
{
    void *ptr;
    struct block *head, *nhead;

    head = a->head;
    if (head->count == head->capa) {
        nhead = block_new(head->capa, a->block_data_size);
        nhead->prev = head;
        a->head = nhead;
        head = nhead;
    }

    ptr = (unsigned char *)head->data + head->count * a->block_data_size;
    head->count++;
    return ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

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
        printf("%d\n", head->v);
        head = next;
    }

    allocator_cleanup(&alloc);
}
