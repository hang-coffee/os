//================================
// main.c - 内核的main
// hangco, 20260926
//================================

#include "include/kprintf.h"
#include "arch/gdt.h"

void kernel_main() {
	gdt_init();
	kprintf("Hello World %d\n", 123);
	while(1);
}

