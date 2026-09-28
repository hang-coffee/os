//================================
// main.c - 内核的main
// hangco, 20260926
//================================

#include "include/kprintf.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/io.h"
#include "mm/mem_map.h"
#include "mm/multiboot.h"
#include "mm/pmm.h"

#include <stdint.h>

void kernel_main(uint32_t magic, uint32_t info) {
	if(magic!=MULTIBOOT_BOOTLOADER_MAGIC) {
		kprintf("Bad bootloader magic: expected 0x%x got 0x%x\n");
		return;
	}
	mmap_init(info);
	pmm_init();
	gdt_init();
	cli();
	idt_init();
	kprintf("Initialized GDT & IDT.\n");
	mmap_print_avail();
	kprintf("Total pages: %d\nFree: %d / Used: %d\n", pmm_total_pages(), pmm_free_pages_count(), pmm_used_pages());
	while(1);
}

