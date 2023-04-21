#define _GNU_SOURCE
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

typedef int (*jit_func)(int, int);

int main(void)
{
    int prots = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;

    void *addr = mmap(NULL, 1024, prots, flags, -1, 0);
    if (addr == MAP_FAILED)
        err(2, "mmap");

    uint8_t code[] = { 
        0x01, 0xf7,   /* add edi, esi */
        0x89, 0xf8,   /* mov eax, edi */
        0xc3,         /* ret */
    };
    memcpy(addr, code, sizeof(code));

    if (mprotect(addr, 1024, PROT_READ | PROT_EXEC) == -1)
        err(2, "mprotect");

    jit_func add_func = addr;
    int result = add_func(10, 19);
    printf("%d\n", result);

    munmap(addr, 1024);
}
