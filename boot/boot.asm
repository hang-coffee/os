;================================
; boot/boot.asm - 启动代码
;================================

KERNEL_BASE equ 0xC0000000

section .multiboot
align 4
	dd 0x1badb002 ; magic
	dd 0x00000003 ; flags
	dd -(0x1badb002 + 0x00000003) ; 校验和

section .boot
bits 32
align 16
extern kernel_main
global _start
	_start:
	cli
	mov esp, boot_stack_top

	mov [boot_magic], eax
	mov [boot_info], ebx

	mov eax, boot_page_dir
	mov cr3, eax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	jmp _start_high

global _start_high
	_start_high:
	mov esp, kernel_stack
	
	extern __bss_start
	extern __bss_end
	mov edi, __bss_start
	mov ecx, __bss_end
	sub ecx, edi
	xor eax, eax
	rep stosb

	mov eax, [boot_magic]
	mov ebx, [boot_info]
	push ebx					; info指针
	push eax					; magic
	call kernel_main
	add esp, 8
	halt_end:
	hlt
	jmp halt_end

align 4096
	; 恒等映射
	boot_page_dir:
	dd boot_pt0 + 0x003
	dd boot_pt1 + 0x003
	dd boot_pt2 + 0x003
	dd boot_pt3 + 0x003
;	dd boot_page_table + 0x003
	times 764 dd 0
	dd boot_pt0 + 0x003
	dd boot_pt1 + 0x003
	dd boot_pt2 + 0x003
	dd boot_pt3 + 0x003
;	dd boot_page_table + 0x003			; PDE 768 (0x300)：0xC0000000-0xC0400000 → 物理 0-4MB
	times 252 dd 0

align 4096
align 4096
boot_pt0:
	%assign i 0
	%rep 1024
		dd (i * 0x1000) + 0x003
		%assign i i+1
	%endrep
boot_pt1:
	%assign i 0
	%rep 1024
		dd (0x400000 + i * 0x1000) + 0x003
		%assign i i+1
	%endrep
boot_pt2:
	%assign i 0
	%rep 1024
		dd (0x800000 + i * 0x1000) + 0x003
		%assign i i+1
	%endrep
boot_pt3:
	%assign i 0
	%rep 1024
		dd (0xC00000 + i * 0x1000) + 0x003
		%assign i i+1
	%endrep

align 4
	boot_magic: dd 0
	boot_info:  dd 0

align 16
	boot_stack:
	resb 4096
	boot_stack_top:

	section .stack
align 16
	global kernel_stack
	resb 16384
	kernel_stack: