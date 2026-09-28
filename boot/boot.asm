;================================
; boot/boot.asm - 启动代码
;================================

section .multiboot
align 4
	dd 0x1badb002 ; magic
	dd 0x00000003 ; flags
	dd -(0x1badb002 + 0x00000003) ; 校验和

section .text
bits 32
align 4
extern kernel_main
global kernel_stack
global _start
	_start:
	cli
	mov edx, kernel_stack
	mov esp, edx
	push ebx					; info指针
	push eax					; magic
	call kernel_main
	add esp, 8
	halt_end:
	hlt
	jmp halt_end

section .bss
align 4
	resb 16384
	kernel_stack:

