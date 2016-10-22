// Note: x86_64_only

#define SAVE_REGS \
    push %rdi;\
    push %rbp;\
    push %rbx;\
    push %r12;\
    push %r13;\
    push %r14;\
    push %r15;\
    sub $8, %rsp;\
    stmxcsr 4(%rsp);\
    fnstcw (%rsp);

#define RESTORE_REGS \
    fldcw (%rsp);\
    ldmxcsr 4(%rsp);\
    add $8, %rsp;\
    pop %r15;\
    pop %r14;\
    pop %r13;\
    pop %r12;\
    pop %rbx;\
    pop %rbp;\
    pop %rdi;

.global _swapstacks
_swapstacks:
    SAVE_REGS
    mov %rsp, (%rdi)
    mov (%rsi), %(rsp)
    RESTORE_REGS
    mov %rdx, %rax
    ret

.global _makestack
_makestack:
    mov %rdx, %r12
    mov %rcx, %r13

    SAVE_REGS
    mov %rsp, (%rdi)
    mov (%rsi), %rsp

    push %rdx
    push %rcx

    //pass args by rdi, rsi, rdx, rcx
    mov %rdi, %rax
    mov %rsi, %rdi
    mov %rax, %rsi
    call _swapstacks
    
    pop %rdi
    pop %r12

    mov %rax, %rsi
    call *%r12

    hlt

