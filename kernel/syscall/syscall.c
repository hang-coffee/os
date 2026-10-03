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
// syscall.c - 系统调用
// hangco, 20261001
//===============================

#include "syscall.h"
#include "../include/errno.h"
#include <stddef.h>
#include <stdint.h>
#include "../include/kprintf.h"

#include "lifecycle.h"
#include "sys_write.h"

static syscall_fn_t syscall_table[256];

int syscall_handler(syscall_regs_t *r) {
//    kprintf("syscall: num=%u, ebx=%x, ecx=%x, edx=%x, cs=%x\n",
//        r->eax, r->ebx, r->ecx, r->edx, r->cs);
    uint32_t num=r->eax;
    if(num>=256 || syscall_table[num]==NULL) return -ENOSYS;
    return syscall_table[num](r);
}

void syscall_register(int num, syscall_fn_t fn) {
    if(num>=0 && num<256) {
        syscall_table[num]=fn;
    }
}

void syscall_init() {
    idt_set_gate(0x80, (uint32_t)syscall_entry, 0x08, 0xEE);
    syscall_register(SYS_EXIT, sys_exit);
    syscall_register(SYS_WRITE, sys_write);
}