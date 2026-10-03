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
// idt.c - IDT相关代码
// hangco, 20260927
//================================

#include "idt.h"
#include "../include/kprintf.h"
#include "io.h"
#include "../mm/paging.h"
#include "../mm/pmm.h"
#include "../mm/page_fault.h"

#pragma GCC target("no-80387")

#define IDT_GATES 256

static idt_gate_t idt_gates[IDT_GATES];
static idt_ptr_t idt_ptr;

typedef enum {
    IDT_INT_GATE=0x8e,
    IDT_TRAP_GATE=0x8f,
    IDT_INT_GATE_R3=0xee,
    IDT_TRAP_GATE_R3=0xef,
    IDT_GATE_UNPRESENT=0x0e,
} idt_attr;

int idt_set_gate(uint32_t index, uint32_t offset, uint16_t selector, uint8_t attr) {
    if(index>=IDT_GATES) return -1;
    idt_gates[index].offset_low=(uint16_t)(offset&0x0000ffff);
    idt_gates[index].offset_high=(uint16_t)((offset&0xffff0000)>>16);
    idt_gates[index].selector=selector;
    idt_gates[index].attr=attr;
    idt_gates[index].rsvd=0;
    return 0;
}

#define DECLARE_ISR(n) extern void isr##n(void);
#define SET_ISR(n) idt_set_gate(n, (uint32_t)isr##n, 0x08, 0x8e);
DECLARE_ISR(0)
DECLARE_ISR(1)
DECLARE_ISR(2)
DECLARE_ISR(3)
DECLARE_ISR(4)
DECLARE_ISR(5)
DECLARE_ISR(6)
DECLARE_ISR(7)
DECLARE_ISR(8)
DECLARE_ISR(9)
DECLARE_ISR(10)
DECLARE_ISR(11)
DECLARE_ISR(12)
DECLARE_ISR(13)
DECLARE_ISR(14)
DECLARE_ISR(15)
DECLARE_ISR(16)
DECLARE_ISR(17)
DECLARE_ISR(18)
DECLARE_ISR(19)
DECLARE_ISR(20)
DECLARE_ISR(21)
DECLARE_ISR(22)
DECLARE_ISR(23)
DECLARE_ISR(24)
DECLARE_ISR(25)
DECLARE_ISR(26)
DECLARE_ISR(27)
DECLARE_ISR(28)
DECLARE_ISR(29)
DECLARE_ISR(30)
DECLARE_ISR(31)

void idt_init(void) {
    for(int i=0; i<IDT_GATES; i++) {
        idt_set_gate(i, (uint32_t)isr_blank, 0x08, IDT_INT_GATE);
    }
    SET_ISR(0);
    SET_ISR(1);
    SET_ISR(2);
    SET_ISR(3);
    SET_ISR(4);
    SET_ISR(5);
    SET_ISR(6);
    SET_ISR(7);
    SET_ISR(8);
    SET_ISR(9);
    SET_ISR(10);
    SET_ISR(11);
    SET_ISR(12);
    SET_ISR(13);
    SET_ISR(14);
    SET_ISR(15);
    SET_ISR(16);
    SET_ISR(17);
    SET_ISR(18);
    SET_ISR(19);
    SET_ISR(20);
    SET_ISR(21);
    SET_ISR(22);
    SET_ISR(23);
    SET_ISR(24);
    SET_ISR(25);
    SET_ISR(26);
    SET_ISR(27);
    SET_ISR(28);
    SET_ISR(29);
    SET_ISR(30);
    SET_ISR(31);
    idt_ptr.size=sizeof(idt_gates)-1;
    idt_ptr.offset=(uint32_t)&idt_gates;
    idt_flush((uint32_t)&idt_ptr);
    return;
}

void isr_handle_de(idt_regs_t *r) {
    kprintf("Divided by zero\n");
    cli();
    hlt();
    return;
}

void isr_handle_df(idt_regs_t *r) {
    kprintf("Double fault\n");
    cli();
    hlt();
    return;
}

void isr_handle_gp(idt_regs_t *r) {
    kprintf("General Protection Err\n");
    cli();
    hlt();
    return;
}

void isr_handle_default_err(idt_regs_t *r) {
    kprintf("An unhandled exception happened.\n");
}

void isr_handler(idt_regs_t *r) {
//    if(idt_user_mode(r)) {
//        kprintf("int %u, eip=0x%x, cs=0x%x\n", r->int_no, r->eip, r->cs);
//        kprintf("    from user: useresp=0x%x, ss=0x%x\n", r->useresp, r->ss);
//    } else {
//        kprintf("    from kernel\n");
//    }

    switch(r->int_no) {
        case 0:                 // #DE -- divided by zero
            isr_handle_de(r);
            break;
        case 8:                 // #DF -- double fault
            isr_handle_df(r);
            break;
        case 13:                // #GP -- general protection
            isr_handle_gp(r);
            break;
        case 14:                // #PF -- page fault
            isr_handle_pf(r);
            break;
        case 0xffffffff:
            kprintf("Default ISR\n");
            break;
        default:
            isr_handle_default_err(r);
    }
    return;
}
