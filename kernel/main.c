//================================
// main.c - 内核的main
// hangco, 20260926
//================================

#include "include/kprintf.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/io.h"

void kernel_main() {
	gdt_init();
	cli();
	idt_init();
	kprintf("Initialized GDT & IDT.\n");
	while(1);
}

