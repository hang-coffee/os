//================================
// gdt.c - GDT定义
// hangco, 20260927
//================================

#include "gdt.h"

#define GDT_ENTRIES 5

static gdt_entry_t gdt_entries[GDT_ENTRIES];
static gdt_ptr_t gdt_ptr;

int gdt_set_entry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    if(limit>0xfffff) return -1;                    // 大于20位
    gdt_entries[index].limit=(uint16_t)(limit&0x0000ffff);
    gdt_entries[index].limit_high_flags=(uint8_t)(((limit&0x00ff0000)>>16)|((flags&0xf)<<4));
    gdt_entries[index].base_high=(uint8_t)((base&0xff000000)>>24);
    gdt_entries[index].base_mid=(uint8_t)((base&0x00ff0000)>>16);
    gdt_entries[index].base_low=(uint16_t)((base&0x0000ffff));
    gdt_entries[index].access=access;
    return 0;
}

void gdt_init(void) {
    gdt_ptr.limit=sizeof(gdt_entries)-1;
    gdt_ptr.base=(uint32_t)&gdt_entries;

    // 填充gdt_entries
    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0x00000000, 0xfffff, 0x9a, 12);
    gdt_set_entry(2, 0x00000000, 0xfffff, 0x92, 12);
    gdt_set_entry(3, 0x00000000, 0xfffff, 0xfa, 0xc);
    gdt_set_entry(4, 0x00000000, 0xfffff, 0xf2, 0xc);

    gdt_flush((uint32_t)&gdt_ptr);
}
