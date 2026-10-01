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
        pic_eoi(irq);
    }
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
