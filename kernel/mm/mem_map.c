//================================
// mem_map.c - 解析内存
// hangco, 20260927
//================================

#include "mem_map.h"
#include "multiboot.h"
#include "../include/kprintf.h"

#include <stdint.h>

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];

mem_region_t mem_map[MAX_MEM_REGIONS];
int mem_region_count=0;
uint32_t mem_total_avail=0;

void mmap_init(uint32_t info) {
	multiboot_info_t *mbi;
	mbi=(multiboot_info_t *)info;
	if(mbi->flags&1) {
		kprintf("Total memory: %d MiB + %d KiB\n", (mbi->mem_lower+mbi->mem_upper)/1024, (mbi->mem_lower+mbi->mem_upper)%1024);
	} else {
		kprintf("Unable to get total memory\n");
	}
	if(mbi->flags&0x40) {
		uint8_t *p = (uint8_t *)(uintptr_t)mbi->mmap_addr;
		uint8_t *end = p + mbi->mmap_length;
		while (p < end) {
 		    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)p;
            switch(mmap->type) {
                case MULTIBOOT_MEMORY_AVAILABLE:
                mmap_add(mmap->addr, mmap->len, MEM_REGION_TYPE_AVAIL);
                mem_total_avail+=mmap->len;
                break;
                case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                mmap_add(mmap->addr, mmap->len, MEM_REGION_TYPE_ACPI_RECL);
                break;
                case MULTIBOOT_MEMORY_NVS:
                mmap_add(mmap->addr, mmap->len, MEM_REGION_TYPE_ACPI_NVS);
                break;
                case MULTIBOOT_MEMORY_BADRAM:
                mmap_add(mmap->addr, mmap->len, MEM_REGION_TYPE_BAD);
                break;
                case MULTIBOOT_MEMORY_RESERVED:
                default:
                mmap_add(mmap->addr, mmap->len, MEM_REGION_TYPE_RSVD);
            }
 			p+=mmap->size+4;   // size 字段自身不计入 size
		}
	}
}

void mmap_add(uint32_t addr, uint32_t len, uint32_t type) {
    if(mem_region_count >= MAX_MEM_REGIONS) return;
    mem_map[mem_region_count].base=addr;
    mem_map[mem_region_count].length=len;
    mem_map[mem_region_count].type=type;
    mem_region_count++;
}

void mmap_print_avail() {
    kprintf("memory region count: %d\n", mem_region_count);
    for(int i=0; i<mem_region_count; i++) {
        if(mem_map[i].type==MEM_REGION_TYPE_AVAIL) {
            kprintf("avail memory region at 0x%08x+0x%08x\n", mem_map[i].base, mem_map[i].length);
        }
    }
}
