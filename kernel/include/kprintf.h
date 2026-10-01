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
// kprintf.h - 内核printf输出
// hangco, 20260926
//================================

#ifndef KPRINTF_H
#define KPRINTF_H

#include <stdarg.h>
#include <stddef.h>
#include "../drivers/vga.h"
#include "../drivers/serial.h"

// 返回值：成功为输出字符数，失败为-1

typedef struct {
    vga_context_t vga;
    serial_context_t serial;
}putc_context_t;

typedef void (*putc_fn)(void *ctx, char c); // 输出回调
int kvprintf(putc_fn putc, void *ctx, const char *fmt, va_list ap);
int kprintf(const char *fmt, ...);
int kpanic(const char *fmt, ...);

#endif
