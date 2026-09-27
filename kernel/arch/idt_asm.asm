;================================
; idt_asm.asm - idt_flush()等代码
; hangco, 20260927
;================================

bits 32
section .text
extern isr_handler
global idt_flush

    idt_flush:
    ; void idt_flush(uint32_t ptr_addr);
    mov eax, [esp+4]
    lidt [eax]
    ret

    isr_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    call isr_handler
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iretd

    %macro ISR_NOERR 1
    global isr%1
    isr%1:
    cli
    push 0
    push %1
    jmp isr_common
    %endmacro

    %macro ISR_ERR 1
    global isr%1
    isr%1:
    cli
    push %1
    jmp isr_common
    %endmacro

    ISR_NOERR 0
    ISR_NOERR 1
    ISR_NOERR 2
    ISR_NOERR 3
    ISR_NOERR 4
    ISR_NOERR 5
    ISR_NOERR 6
    ISR_NOERR 7
    ISR_ERR   8
    ISR_NOERR 9
    ISR_ERR   10
    ISR_ERR   11
    ISR_ERR   12
    ISR_ERR   13
    ISR_ERR   14
    ISR_NOERR 15
    ISR_NOERR 16
    ISR_ERR   17
    ISR_NOERR 18
    ISR_NOERR 19
    ISR_NOERR 20
    ISR_NOERR 21
    ISR_NOERR 22
    ISR_NOERR 23
    ISR_NOERR 24
    ISR_NOERR 25
    ISR_NOERR 26
    ISR_NOERR 27
    ISR_NOERR 28
    ISR_NOERR 29
    ISR_ERR   30
    ISR_NOERR 31
