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
// ustack.h - 用户栈
// hangco, 20261002
//===============================

#ifndef USTACK_H
#define USTACK_H

#include "layout.h"

#define STACK_PAGE_OF(addr) ((addr)&(~0xfff))
#define IS_IN_STACK_RANGE(t, addr) \
    ((addr)>=USER_STACK_LIMIT && (addr)<(t)->stack_top)
#define STACK_IS_EXPANDING(t, addr) \
    (IS_IN_STACK_RANGE(t, addr)&&(addr)<(t)->stack_limit)

#include "../sched/task.h"

int ustack_setup(task_t *t);    // 设置用户栈
int ustack_expand(task_t *t, uint32_t addr);    // 从addr开始扩展用户栈

#endif
