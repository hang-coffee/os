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
global _start
	_start:
	cli
	mov eax, kernel_stack
	mov esp, eax
	push eax					; magic
	push ebx					; info指针
	call kernel_main
	halt_end:
	hlt
	jmp halt_end

section .bss
align 4
	resb 16384
	kernel_stack:

