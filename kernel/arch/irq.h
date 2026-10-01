//===============================
// irq.h - IRQ处理
// hangco, 20261001
//===============================

#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>
#include "idt.h"    // 获得regs
typedef idt_regs_t irq_regs_t;
typedef void (*irq_handler_t)(irq_regs_t *);

extern uint32_t irq_stub_table[16];

void irq_init(void);
void irq_handler(irq_regs_t *r);
void irq_register_handler(uint8_t irq, irq_handler_t fn);
void irq_unregister_handler(uint8_t irq);

#endif
