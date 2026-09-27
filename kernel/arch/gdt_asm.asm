;================================
; gdt_asm.asm - gdt_flush()等代码
; hangco, 20260927
;================================

section .text
bits 32
global gdt_flush
extern gdt_ptr
    ; void gdt_flush(uint32_t ptr_addr);
    gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
    .flush:
    ret
