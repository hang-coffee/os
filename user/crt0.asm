BITS 32

section .text
global _start
extern main

_start:
    ; main(argc, argv)
    mov eax, [esp]          ; argc
    lea ebx, [esp + 4]      ; argv
    push ebx
    push eax
    call main
    add esp, 8

    ; exit(main 的返回值)
    mov ebx, eax
    mov eax, 1              ; SYS_EXIT
    int 0x80

    hlt
    jmp $