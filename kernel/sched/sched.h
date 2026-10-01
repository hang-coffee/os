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
// sched.h - 调度器
// hangco, 20261001
//===============================

#ifndef SCHED_H
#define SCHED_H

#include "task.h"
#include "../arch/irq.h"
#include <stdint.h>

void scheduler_init();
int scheduler_add_task(task_t *task);
int scheduler_remove_task(task_t *task);
task_t *scheduler_pick_next(void);
void schedule();
void scheduler_tick(irq_regs_t *r);
void sleep_queue_add(task_t *task);
int sleep_queue_remove(task_t *task);
void task_sleep(task_t *task);
void sleep_ms(uint32_t ms);
void task_block(int reason);
void task_unblock(task_t *task);

#endif
