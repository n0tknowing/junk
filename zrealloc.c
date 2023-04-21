#define NULL       ((void *)0UL)
#define ZR_PRIV    __attribute__((visibility ("hidden")))
#define MIN(x, y)  ((x) < (y) ? (x) : (y))

typedef unsigned long size_t;
typedef unsigned char u8;

struct zMem {
    size_t size;
};

ZR_PRIV void *zMemRealloc(void *ptr, size_t nmemb, size_t size)
{
    u8 *nptr = __builtin_calloc(1UL, sizeof(struct zMem) + nmemb * size);
    if (nptr == NULL)
        return NULL;
    struct zMem *zm = (struct zMem *)nptr;
    zm->size = nmemb * size;
    nptr += sizeof(struct zMem);
    if (ptr != NULL) {
        struct zMem *zm2 = (struct zMem *)((u8 *)ptr - sizeof(struct zMem));
        __builtin_memcpy(nptr, ptr, MIN(zm->size, zm2->size));
        __builtin_free(zm2);
    }
    return nptr;
}

ZR_PRIV void *zMemAlloc(size_t nmemb, size_t size)
{
    return zMemRealloc(NULL, nmemb, size);
}

ZR_PRIV void zMemFree(void *ptr)
{
    if (ptr != NULL)
        __builtin_free((u8 *)ptr - sizeof(struct zMem));
}
