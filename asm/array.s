    .text
    .globl main
    .type main, @function
main:
    push %rbp
    movq %rsp, %rbp
    subq $8000, %rsp
    xorq %rcx, %rcx
.loop_jl:
    movq %rcx, (%rsp,%rcx,4)
    incq %rcx
    cmpq $2000, %rcx
    jl .loop_jl
    movq 968(%rsp), %rax
    addq $8000, %rsp
    pop %rbp
    ret
