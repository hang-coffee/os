;===============================
; tss_asm.asm - 任务
; hangco, 20261001
;===============================

bits 32
section .text
global tss_flush

    tss_flush:
    mov ax, 0x28
    ltr ax
    ret
