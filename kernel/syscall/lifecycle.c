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
// lifecycle.c - 生命周期类
// hangco, 20261001
//===============================

#include "lifecycle.h"
#include "../sched/task.h"
#include "../drivers/pit.h"
#include "../mm/paging.h"
#include "../mm/pmm.h"
#include <stddef.h>
#include <stdint.h>

int sys_exit(syscall_regs_t *r) {    // ebx=status
    int status=(int)(r->ebx & 0xff);
    task_exit(status);
    __builtin_unreachable();
    return 0;
}
