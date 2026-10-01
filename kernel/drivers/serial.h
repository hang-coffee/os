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
// serial.h - 串口驱动
// hangco, 20260926
//================================

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

typedef struct {
    uint16_t base_port;
} serial_context_t;

void serial_init(serial_context_t *serial, uint16_t base);
void serial_wait_empty(serial_context_t *serial);
void serial_wait_data_ready(serial_context_t *serial);
void serial_putc(serial_context_t *serial, uint8_t c);
void serial_puts(serial_context_t *serial, char *s);
// TODO: 还有一个ISR，现阶段不需要写

#endif
