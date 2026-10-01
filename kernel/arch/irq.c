//===============================
// irq.c - IRQ的处理
// hangco, 20261001
//===============================

#include "irq.h"
#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "pic.h"
#include "../include/kprintf.h"

static irq_handler_t irq_handlers[16]={0};

void irq_init(void) {
    for(int i=0; i<16; i++) {
        idt_set_gate(0x20+i, irq_stub_table[i], 0x08, 0x8e);
    }
}

void irq_handler(irq_regs_t *r) {
    uint8_t irq=r->int_no-0x20;
    if(irq>15) return;
    if(irq_handlers[irq]) {
        irq_handlers[irq](r);
    } else {
        kprintf("ERR: unregistered IRQ @ %u\n", irq);
    }
    pic_eoi(irq);
    return;
}

void irq_register_handler(uint8_t irq, irq_handler_t fn) {
    irq_handlers[irq]=fn;
    return;
}

void irq_unregister_handler(uint8_t irq) {
    irq_handlers[irq]=NULL;
    return;
}
