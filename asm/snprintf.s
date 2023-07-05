    .text
    .globl main
    .type main, @function
main:
    pushq %rbx
    pushq %r12
    pushq %r13
    subq $40, %rsp
    xorl %r12d, %r12d
    xorl %r13d, %r13d
    movq %rsp, %rbx
.L0:
    movl %r12d, %ecx
    leaq .S1(%rip), %rdx
    movl $32, %esi
    movq %rbx, %rdi
    xorl %eax, %eax
    call snprintf
    addl %eax, %r13d
    addl $1, %r12d
    cmpl $15, %r12d
    jl .L0
    leaq .S2(%rip), %rdi
    movl %r13d, %esi
    xorl %eax, %eax
    call printf
    addq $40, %rsp
    popq %r13
    popq %r12
    popq %rbx
    ret
    .section .rodata
.S1:
    .string "buf-%02d"
.S2:
    .string "sum = %d\n"
