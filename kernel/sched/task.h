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
// task.h - 任务结构
// hangco, 20261001
//===============================

#ifndef TASK_H
#define TASK_H

#include <stdint.h>

enum task_state {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_ZOMBIE,
};

#define KERNEL_STACK_SIZE 8192
#define DEFAULT_TICKS 1

typedef struct task {
    uint32_t pid;
    uint32_t ppid;
    uint32_t uid;
    uint32_t euid;
    uint32_t gid;
    uint32_t egid;
    uint32_t brk;
    uint32_t *pgdir;
    uint32_t brk_start;
    uint32_t stack_top;
    uint32_t stack_limit;   // 用户栈
    char name[32];
    enum task_state state;
    uint32_t esp;       // 切换时保存的栈指针
    uint32_t kernel_stack_base; // 栈底，释放时使用
    uint32_t kernel_stack_size;
    uint32_t page_dir_phys; // 分页相关
    uint32_t ticks_left;    // 剩余时间片
    uint32_t priority;      // TODO: 增加真正的对优先级的管理
    uint64_t wakeup_tick;   // 睡眠用
    int block_reason;
    struct task *next;      // 就绪队列
    struct task *next_sleep;    // 睡眠队列
    int exit_code;
    uint64_t exit_time;
    uint32_t user_entry;    // 用户入口线性地址
    uint32_t user_esp;      // 用户栈顶线性地址
} task_t;

extern task_t *current;
extern task_t *idle_task;

typedef void (*task_entry_t)(void);

task_t *task_create(const char *name, task_entry_t entry);
int task_destroy(task_t *task);
void task_exit(int code);       // 当前任务主动退出
void idle_entry(void);
task_t *task_get_current(void);
void task_set_current(task_t *task);
char *task_state_name(enum task_state state);
void task_dump(task_t *task);
void task_init();

extern void switch_context(uint32_t *old_esp, uint32_t new_esp);

task_t *task_create_user(const char *name, const uint8_t *elf, uint32_t size);

#endif
