#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static size_t align_up(size_t base, size_t align)
{
    align--;
    return (base + align) & ~align;
}

size_t compute_struct_size(size_t *alignments, size_t n)
{
    if (n == 1)
        return alignments[0];

    size_t size = 0, align = 4;

    for (size_t i = 0; i < n; i++) {
        size += alignments[i];
        if (size % alignments[i] != 0)
            size = align_up(size, alignments[i] <= 8 ? alignments[i] : 8);
        if (alignments[i] == 8)
            align = 8;
    }

    return align_up(size, align);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    size_t n = (size_t)argc - 1;
    size_t *alignments = calloc(n, sizeof(size_t));
    assert(alignments != NULL);

    for (size_t i = 0; i < n; i++)
        alignments[i] = atoi(argv[i + 1]);

    size_t size = compute_struct_size(alignments, n);
    printf("%zu\n", size);

    free(alignments);
}
