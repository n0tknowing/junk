/*
 *  int main(int argc __unused, char **argv) {
 *      while (*argv) {
 *          puts(*argv++);
 *      }
 *  }
 */
    .text
    .globl main
    .type main, @function
main:
    pushq %rbx
    movq %rsi, %rbx
    cmpq $0, %rbx
    je .L0
.L1:
    movq (%rbx), %rdi
    call puts
    addq $8, %rbx
    cmpq $0, (%rbx)
    jne .L1
.L0:
    xorl %eax, %eax
    popq %rbx
    ret
