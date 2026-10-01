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
// kvprintf.c - 内核printf输出
// hangco, 20260926
//================================

#include "../include/kprintf.h"
#include <stdint.h>

static void emit(putc_fn putc, void *ctx, char c, int *count) {
    putc(ctx, c);
    (*count)++;
}

static void emit_str(putc_fn putc, void *ctx, const char *s, int *count) {
    if (!s) s = "(null)";
    while (*s) emit(putc, ctx, *s++, count);
}

static void emit_uint(putc_fn putc, void *ctx,
                      uint32_t v, uint32_t base, int upper,
                      int width, int zero_pad, int left_align,
                      int *count) {
    char buf[32];
    const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    int i = 0;

    if (v == 0) buf[i++] = '0';
    while (v) {
        buf[i++] = digits[v % base];
        v /= base;
    }

    int pad = width - i;
    if (!left_align) {
        while (pad-- > 0) emit(putc, ctx, pad >= 0 && zero_pad ? '0' : ' ', count);
    }
    while (i > 0) emit(putc, ctx, buf[--i], count);
    if (left_align) {
        while (pad-- > 0) emit(putc, ctx, ' ', count);
    }
}

int kvprintf(putc_fn putc, void *ctx, const char *fmt, va_list ap) {
    if (!fmt || !putc) return -1;

    int count = 0;
    while (*fmt) {
        if (*fmt != '%') { emit(putc, ctx, *fmt++, &count); continue; }
        fmt++;  /* 跳过 % */

        /* 解析 flags */
        int left_align = 0, zero_pad = 0, plus = 0, space = 0, hash = 0;
        for (;; fmt++) {
            if (*fmt == '-') left_align = 1;
            else if (*fmt == '0') zero_pad = 1;
            else if (*fmt == '+') plus = 1;
            else if (*fmt == ' ') space = 1;
            else if (*fmt == '#') hash = 1;
            else break;
        }

        /* 解析宽度 */
        int width = 0;
        while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');

        /* 解析精度（可选） */
        int prec = -1;
        if (*fmt == '.') {
            fmt++;
            prec = 0;
            while (*fmt >= '0' && *fmt <= '9') prec = prec * 10 + (*fmt++ - '0');
        }

        /* 解析长度 */
        int is_long = 0, is_short = 0;
        if (*fmt == 'l') { is_long = 1; fmt++; if (*fmt == 'l') { is_long = 2; fmt++; } }
        else if (*fmt == 'h') { is_short = 1; fmt++; if (*fmt == 'h') { is_short = 2; fmt++; } }
        else if (*fmt == 'z') { is_long = 1; fmt++; }

        /* 分发 */
        char spec = *fmt;
        if (spec == '\0') break;

        switch (spec) {
            case 'd': case 'i': {
                long v = is_long ? va_arg(ap, long) : va_arg(ap, int);
                uint32_t u;
                if (v < 0) { emit(putc, ctx, '-', &count); u = (uint32_t)(-(v + 1)) + 1; }
                else { if (plus) emit(putc, ctx, '+', &count);
                       else if (space) emit(putc, ctx, ' ', &count);
                       u = (uint32_t)v; }
                emit_uint(putc, ctx, u, 10, 0, width - (v < 0), zero_pad, left_align, &count);
                break;
            }
            case 'u': case 'x': case 'X': case 'o': case 'b': {
                uint32_t u = is_long ? va_arg(ap, unsigned long)
                                     : va_arg(ap, unsigned int);
                uint32_t base = (spec == 'o') ? 8 : (spec == 'b') ? 2 : (spec == 'u') ? 10 : 16;
                if (hash && (spec == 'x' || spec == 'X')) {
                    emit(putc, ctx, '0', &count);
                    emit(putc, ctx, spec, &count);
                }
                emit_uint(putc, ctx, u, base, spec == 'X', width, zero_pad, left_align, &count);
                break;
            }
            case 'p': {
                uintptr_t p = (uintptr_t)va_arg(ap, void*);
                emit(putc, ctx, '0', &count);
                emit(putc, ctx, 'x', &count);
                emit_uint(putc, ctx, (uint32_t)p, 16, 0, 8, 1, 0, &count);
                break;
            }
            case 's': {
                const char *s = va_arg(ap, const char*);
                emit_str(putc, ctx, s, &count);
                break;
            }
            case 'c': {
                char c = (char)va_arg(ap, int);
                emit(putc, ctx, c, &count);
                break;
            }
            case '%': {
                emit(putc, ctx, '%', &count);
                break;
            }
            default:
                emit(putc, ctx, '%', &count);
                emit(putc, ctx, spec, &count);
                break;
        }
        fmt++;
    }
    return count;
}
