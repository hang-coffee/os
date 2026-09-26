//================================
// main.c - 内核的main
// hangco, 20260926
//================================

#include "include/kprintf.h"

void kernel_main() {
	kprintf("Hello World %d\n", 123);
	while(1);
}

