/*
 * This file is part of Congestus.
 * Copyright (C) 2026 hangco
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

//================================
// main.c - 内核的main
// hangco, 20260926
//================================

#include "include/kprintf.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/io.h"
#include "arch/pic.h"
#include "arch/irq.h"
#include "arch/tss.h"
#include "drivers/pit.h"
#include "mm/mem_map.h"
#include "mm/multiboot.h"
#include "mm/pmm.h"
#include "mm/paging.h"
#include "mm/heap.h"
#include "sched/task.h"
#include "sched/sched.h"
#include "syscall/syscall.h"
#include "syscall/lifecycle.h"
#include "test.h"

#include <stdint.h>

size_t strlen(char arr[]) {
    int i = 0;
    while (arr[i++] != '\0') {
        ; // 空语句
    }
    return i - 1; // 由于i是后置自增，需要减一
}

void kernel_main(uint32_t magic, uint32_t info) {
	gdt_init();
    tss_init();
	idt_init();
	cli();
	if(magic!=MULTIBOOT_BOOTLOADER_MAGIC) {
		kprintf("Bad bootloader magic: expected 0x%x got 0x%x\n", magic, info);
		return;
	}
	mmap_init(info);
	pmm_init();
	mmap_print_avail();
	paging_init();
	heap_init();
	kprintf("Total pages: %d\nFree: %d / Used: %d\n", pmm_total_pages(), pmm_free_pages_count(), pmm_used_pages());

    pic_init();
    irq_init();
    syscall_init();

    scheduler_init();
    task_init();
    pit_init(100);
    pic_unmask(0);
    sti();

    kprintf("\nWelcome to CGST(ConGeSTus' Glued, Silly & Terrible)!\n\n");

    test_usermode_only();

	while(1);
}

