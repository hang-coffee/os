BITS 32
org 0x08048000

section .text
global user_test_start
global user_test_end

user_test_start:
    ; sys_write(1, msg, 13)
    mov eax, 4
    mov ebx, 1
    mov ecx, msg
    mov edx, 13
    int 0x80

    mov esp, 0xBFFFF000
    sub esp, 8192         ; 越过初始映射的 1 页
    mov dword [esp], 0xAA ; 触发 #PF

    ; sys_exit(0)
    mov eax, 1
    mov ebx, 0
    int 0x80

    hlt
    jmp $

msg:
    db "Hello, user!", 10

user_test_end: