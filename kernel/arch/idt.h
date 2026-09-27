//================================
// idt.h - IDT相关代码
// hangco, 20260927
//================================

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t rsvd;
    uint8_t attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_gate_t;

typedef struct {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed)) idt_ptr_t;

void idt_init(void);
int idt_set_gate(uint32_t index, uint32_t offset, uint16_t selector, uint8_t attr);
extern void idt_flush(uint32_t ptr_addr);

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
    uint32_t useresp, ss;
} __attribute__((packed)) idt_regs_t;

#define idt_user_mode(r) (((r)->cs & 3)==3)
void isr_handler(idt_regs_t *r);

#endif
