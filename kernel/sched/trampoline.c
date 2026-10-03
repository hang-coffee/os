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
// trampoline.c - ring3蹦床
// hangco, 20261003
//===============================

#include "trampoline.h"
#include "task.h"

void user_trampoline(void) {
    uint32_t user_eip=current->user_entry;
    uint32_t user_esp=current->user_esp;
    __asm__ volatile (
        ".intel_syntax noprefix\n"
        "cli\n"
        "mov ax, 0x23\n"    // 用户数据段
        "mov ds, ax\n"
        "mov es, ax\n"
        "mov fs, ax\n"
        "mov gs, ax\n"
        "push 0x23\n"       // ss
        "push %0\n"         // esp
        "push 0x202\n"      // eflags: if=1
        "push 0x1b\n"       // cs
        "push %1\n"         // eip
        "iretd\n"
        ".att_syntax"
        :
        : "r"(user_esp), "r"(user_eip)
        : "eax", "memory"
    );
    __builtin_unreachable();
}
