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
// tss.c - 任务
// hangco, 20261001
//===============================

#include "tss.h"
#include "gdt.h"

static tss_entry_t tss;

extern void *memset(void *s, int c, uint32_t n);

void tss_init(void) {
    memset(&tss, 0, sizeof(tss));
    tss.ss0=0x10;
    tss.iomap_base=sizeof(tss);
    gdt_set_entry(5, (uint32_t)&tss, sizeof(tss)-1, 0x89, 0x00);
    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
}

void tss_set_kernel_stack(uint32_t esp0) {
    tss.esp0=esp0;
}

uint32_t tss_get_kernel_stack(void) {
    return tss.esp0;
}
