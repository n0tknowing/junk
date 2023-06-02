/*
 *  int main(int argc, char **argv) {
 *      int i;
 *      for (i = 1; i < argc; i++)
 *          puts(argv[i]);
 *  }
 */
    .text
    .globl main
    .type main, @function
main:
    pushq %rbx
    pushq %r14
    pushq %r15
    movq %rsi, %rbx
    movl $1, %r14d
    movl %edi, %r15d
    cmpl %r15d, %r14d
    jge .L0
.L1:
    movq (%rbx,%r14,8), %rdi
    call puts
    incl %r14d
    cmpl %r15d, %r14d
    jl .L1
.L0:
    xorl %eax, %eax
    popq %r15
    popq %r14
    popq %rbx
    ret
