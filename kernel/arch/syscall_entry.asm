;===============================
; syscall_entry.asm - 系统调用入口
; hangco, 20261001
;===============================

bits 32
section .text
global syscall_entry
extern syscall_handler

    syscall_entry:
    cli
    push 0
    push 0x80
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
    call syscall_handler
    add esp, 4
    mov [esp+44], eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iretd
