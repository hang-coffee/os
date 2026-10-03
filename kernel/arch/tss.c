//===============================
// tss.c - 任务
// hangco, 20261001
//===============================

#include "tss.h"
#include "gdt.h"

static tss_entry_t tss;

extern void *memset(void *s, int c, uint32_t n);

void tss_init(void) {
    memset(&tss, 0, sizeof(tss));
    tss.ss0=0x10;
    tss.iomap_base=sizeof(tss);
    gdt_set_entry(5, (uint32_t)&tss, sizeof(tss)-1, 0x89, 0x00);
    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
}

void tss_set_kernel_stack(uint32_t esp0) {
    tss.esp0=esp0;
}

uint32_t tss_get_kernel_stack(void) {
    return tss.esp0;
}
