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
// serial.c - 串口驱动
// hangco, 20260926
//================================

#include "serial.h"
#include <stdint.h>
#include "../arch/io.h"

void serial_init(serial_context_t *serial, uint16_t base) {
    serial->base_port=base;

    // 硬件层面初始化
    outb(base+1, 0x00); // 关中断
    outb(base+3, 0x80); // DLAB
    outb(base+0, 0x03);
    outb(base+1, 0x00); // 38400 baud
    outb(base+3, 0x03); // 8n1
    outb(base+2, 0xc7); // fifo
    outb(base+4, 0x0b); // rts/dsr
    return;
}

void serial_wait_empty(serial_context_t *serial) {
    while(!(inb(serial->base_port+5)&0x20));
}

void serial_wait_data_ready(serial_context_t *serial) {
    while(!(inb(serial->base_port+5)&0x01));
}

void serial_putc(serial_context_t *serial, uint8_t c) {
    serial_wait_empty(serial);
    outb(serial->base_port+0, c);
}

void serial_puts(serial_context_t *serial, char *s) {
    int i=0;
    while(s[i]!='\0') {
        serial_putc(serial, s[i]);
        i++;
    }
}
