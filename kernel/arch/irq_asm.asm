;===============================
; irq_asm.asm - IRQ处理
; hangco, 20261001
;===============================

section .text
bits 32
global irq_common
extern irq_handler

    irq_common:
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
    call irq_handler
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iretd

%macro IRQ_STUB 1
global irq%1
    
    irq%1:
    cli
    push 0
    push (0x20+%1)
    jmp irq_common
%endmacro

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15

section .data
global irq_stub_table
irq_stub_table:
    dd irq0,  irq1,  irq2,  irq3
    dd irq4,  irq5,  irq6,  irq7
    dd irq8,  irq9,  irq10, irq11
    dd irq12, irq13, irq14, irq15
