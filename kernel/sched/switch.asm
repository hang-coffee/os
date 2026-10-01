;===============================
; switch.asm - 任务切换 汇编
; hangco, 20261001
;===============================

section .text
bits 32
global switch_context

    switch_context:
    ; void switch_context(uint32_t *old_esp, uint32_t new_esp)
    ; 此时，[esp+4]-->old_esp的地址；[esp+8]-->new_esp本身
    push gs
    push fs
    push es
    push ds
    pusha
    pushfd
    mov eax, [esp+56]   ; old_esp
    mov edx, [esp+60]
    mov [eax], esp
    mov esp, edx        ; 保存esp并切换
    popfd
    popa
    pop ds
    pop es
    pop fs
    pop gs
    ret
