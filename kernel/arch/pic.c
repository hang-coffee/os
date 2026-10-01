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
// pic.c - PIC(8259A)的驱动
// hangco, 20261001
//================================

#include "pic.h"
#include "io.h"
#include <stdint.h>

void pic_init(void) {
    uint8_t offset1=0x20, offset2=0x28;
    outb(PIC_MASTER_LOW, 0x11);
    outb(PIC_SLAVE_LOW, 0x11);       // 0x11=边沿触发，需要ICW4
    outb(PIC_MASTER_HIGH, offset1); // 设定向量基址
    outb(PIC_SLAVE_HIGH, offset2);
    outb(PIC_MASTER_HIGH, 0x04);    // 从片IRQ2
    outb(PIC_SLAVE_HIGH, 0x02);
    outb(PIC_MASTER_HIGH, 0x01);
    outb(PIC_SLAVE_HIGH, 0x01);     // 86模式
    outb(PIC_MASTER_HIGH, 0xff);
    outb(PIC_SLAVE_HIGH, 0xff);     // 屏蔽
}

void pic_mask(uint8_t irq) {
    if(irq>15) return;
    uint16_t port; uint8_t bit;
    if(irq<8) {
        port=PIC_MASTER_HIGH;
        bit=irq;
    } else {
        port=PIC_SLAVE_HIGH;
        bit=irq-8;
    }
    uint8_t mask=inb(port);
    mask|=(1<<bit);
    outb(port, mask);
    return;
}

void pic_unmask(uint8_t irq) {
    if(irq>15) return;
    if(irq>=8) pic_unmask(2);
    uint16_t port; uint8_t bit;
    if(irq<8) {
        port=PIC_MASTER_HIGH;
        bit=irq;
    } else {
        port=PIC_SLAVE_HIGH;
        bit=irq-8;
    }
    uint8_t mask=inb(port);
    mask&=~(1<<bit);
    outb(port, mask);
    return;
}

void pic_eoi(uint8_t irq) {
    if(irq>=8) {
        outb(PIC_SLAVE_LOW, 0x20);  // 从片EOI
    }
    outb(PIC_MASTER_LOW, 0x20);
    return;
}

uint8_t pic_get_irr(void) {
    outb(PIC_MASTER_LOW, 0x0a); // OCW3
    return inb(PIC_MASTER_LOW);
}

uint8_t pic_get_isr(void) {
    outb(PIC_MASTER_LOW, 0x0b); // OCW3
    return inb(PIC_MASTER_LOW);
}

uint16_t pic_get_mask(void) {
    return (inb(PIC_SLAVE_HIGH)<<8)|(inb(PIC_MASTER_HIGH));
}

void pic_set_mask(uint8_t master, uint8_t slave) {
    outb(PIC_MASTER_HIGH, master);
    outb(PIC_SLAVE_HIGH, slave);
}
