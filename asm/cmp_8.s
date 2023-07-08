    .text
    .type eq8, @function
eq8:
    movq (%rdi), %rax
    cmpq (%rsi), %rax
    sete %al
    movzx %al, %eax
    ret
    .type ne8, @function
ne8:
    movq (%rdi), %rax
    cmpq (%rsi), %rax
    setne %al
    movzx %al, %eax
    ret
    .globl main
    .type main, @function
main:
    leaq .s0(%rip), %rsi
    leaq .s1(%rip), %rdi
    jmp ne8
    .section .rodata
.s0:
    .string "qwertyup"
.s1:
    .string "qwertyu"
