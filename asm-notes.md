### x86 General Purpose Registers on linux

| 64-bit | 32-bit | 16-bit | 8-bit | notes                       |
|--------|--------|--------|-------|-----------------------------|
|   rsp  | esp    | sp     | spl   | stack pointer, callee-saved |
| rax    | eax    | ax     | al    | return value, caller-saved  |
| rbx    | ebx    | bx     | bl    | callee-saved                |
| rbp    | ebp    | bp     | bpl   | base pointer, callee-saved  |
| rdi    | edi    | di     | dil   | 1st argument, caller-saved  |
| rsi    | rsi    | si     | sil   | 2nd argument, caller-saved  |
| rdx    | edx    | dx     | dl    | 3rd argument, caller-saved  |
| rcx    | ecx    | cx     | cl    | 4th argument, caller-saved  |
| r8     | r8d    | r8w    | r8b   | 5th argument, caller-saved  |
| r9     | r9d    | r9w    | r9b   | 6th argument, caller-saved  |
| r10    | r10d   | r10w   | r10b  | temp register, caller-saved |
| r11    | r11d   | r11w   | r11b  | temp register, caller-saved |
| r12    | r12d   | r12w   | r12b  | callee-saved                |
| r13    | r13d   | r13w   | r13b  | callee-saved                |
| r14    | r14d   | r14w   | r14b  | callee-saved                |
| r15    | r15d   | r15w   | r15b  | callee-saved                |

### Instruction suffixes (AT&T syntax)

- b = byte
- w = word
- l = doubleword
- q = quadword

### Intel syntax with GAS

```asm
    .intel_syntax noprefix

    your code here
```

### General questions and answers

#### What is Caller-saved and Callee-saved?

Caller-saved means that a register may be overwritten after calling
a function.  On the other hand, Callee-saved means a register will
not be overwritten. Another terms for these are call-clobbered and
call-preserved.

Code example:

```asm
    .text
    .type foo, @function
foo:
    movl $1, %esi
    nop
    ret

    .globl main
    .type main, @function
main:
    movl $0, %esi
    leaq .FMT_0(%rip), %rdi
    xorl %eax, %eax
    call printf
    call foo
    leaq .FMT_1(%rip), %rdi
    xorl %eax, %eax
    call printf
    xorl %eax, %eax
    ret

    .section .rodata
.FMT_0:
    .string "before calling foo(), esi = %d\n"
.FMT_1:
    .string "after calling foo(),  esi = %d\n"
```

Output should be:

```
before calling foo(), esi = 0
after calling foo(),  esi = 1
```

As you can see, before calling `foo()`, `esi` is 0 and inside `foo()`, it
modifies `esi` to 1, after the function returned, `esi` is modified (clobbered)
and no longer 0 when it's printed.
