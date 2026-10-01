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
// kprintf.c - 内核printf输出
// hangco, 20260926
//================================

#include "../include/kprintf.h"
#include "../drivers/vga.h"
#include "../drivers/serial.h"

static void console_putc(void *ctx, char c) {
    vga_putc(&((putc_context_t *)ctx)->vga, c, 0x0f);
    serial_putc(&((putc_context_t *)ctx)->serial, c);
}

int kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    putc_context_t ctx={.serial={.base_port=0x3f8}, .vga=vga_get_info()};
    int n = kvprintf(console_putc, &ctx, fmt, ap);
    va_end(ap);
    return n;
}

int kpanic(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    putc_context_t ctx={.serial={.base_port=0x3f8}, .vga=vga_get_info()};
    int n = kvprintf(console_putc, &ctx, fmt, ap);
    va_end(ap);
    for (;;) __asm__ volatile ("cli; hlt");
    return n;
}
