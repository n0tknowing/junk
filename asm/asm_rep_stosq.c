#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_REP_STOSQ
/**
 * With this function, generated assembly is:
 *
 *   sub      $0x1000008, %rsp  ; 's'
 *   mov      $0x20000, %ecx    ; set 'sz', 0x20000 because 8 bytes per loop
 *   mov      %rsp, %rdi        ; set 'p'
 *   xor      %eax, %eax        ; we are going to zero memory
 *   rep stos %rax, %es:(%rdi)  ; repeat until ecx == 0
 */
static void zeromem(void *s, size_t n)
{
    uint8_t *p = s;
    size_t sz = (n + 7) >> 3;
    __asm__ volatile(
        "rep stosq\n"
        : "+c"(sz), "+D"(p)
        : "a"(0)
        : "memory"
    );
}
#else
/**
 * With this function, generated assembly may use SSE/AVX instructions
 */
static void zeromem(void *s, size_t n)
{
    uint8_t *p = s;
    uint64_t *pp;

    while (n >= 32) {
        pp = (uint64_t *)p;
        pp = pp; *pp = 0;
        pp = pp + 1; *pp = 0;
        pp = pp + 1; *pp = 0;
        pp = pp + 1; *pp = 0;
        n -= 32; p += 32;
    }

    while (n > 0)
        *p++ = 0;
}
#endif // USE_REP_STOSQ

int main(void)
{
    char buf[1UL << 20];
    zeromem(buf, sizeof(buf));
    memcpy(buf, "Hello world", sizeof("Hello world") - 1);
    puts(buf);
}
