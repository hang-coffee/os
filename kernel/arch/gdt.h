//================================
// gdt.h - GDT定义
// hangco, 20260926
//================================

#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_high_flags;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t  base;
} __attribute__((packed)) gdt_ptr_t;        // LGDT用的

#endif
