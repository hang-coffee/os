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
// syscall.h - 系统调用
// hangco, 20261001
//===============================

#ifndef SYSCALL_H
#define SYSCALL_H

#include "../arch/idt.h"

#define SYS_EXIT 1

typedef idt_regs_t syscall_regs_t;
typedef int (*syscall_fn_t)(syscall_regs_t *);

int syscall_handler(syscall_regs_t *r);
void syscall_register(int num, syscall_fn_t fn);
void syscall_init();

extern void syscall_entry();

#endif