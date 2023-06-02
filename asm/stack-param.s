    .text
    .type foo, @function
foo:
    movl %edi, %eax
    addl %esi, %eax
    addl %edx, %eax
    addl %ecx, %eax
    addl %r8d, %eax
    addl %r9d, %eax
    addl 8(%rsp), %eax
    addl 16(%rsp), %eax
    addl 24(%rsp), %eax
    addl 32(%rsp), %eax
    ret

    .globl main
    .type main, @function
main:
    pushq $10
    pushq $9
    pushq $8
    pushq $7
    movl $6, %r9d
    movl $5, %r8d
    movl $4, %ecx
    movl $3, %edx
    movl $2, %esi
    movl $1, %edi
    call foo
    movl %eax, %esi
    leaq .S0(%rip), %rdi
    xorl %eax, %eax
    call printf
    xorl %eax, %eax
    addq $32, %rsp
    ret

    .section .rodata
.S0:
    .string "%d\n"
