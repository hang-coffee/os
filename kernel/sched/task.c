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
// task.c - 任务结构
// hangco, 20261001
//===============================

#include "task.h"
#include <stdint.h>
#include "../mm/heap.h"
#include <stddef.h>
#include "../include/errno.h"
#include "../include/kprintf.h"
#include "../arch/io.h"
#include "sched.h"

extern uint32_t kernel_page_dir_phys;

void *memset(void *s, int c, uint32_t n) {    // fills the first n bytes of the memory area pointed to by s with the constant byte c
    uint8_t *p=(uint8_t *)s;
    for (uint32_t i=0; i<n; i++) {
        p[i]=(uint8_t)c;
    }
    return s;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *start = dest;
    while (n > 0 && *src != '\0') {
        *dest++ = *src++;
        n--;
    }
    while (n > 0) {
        *dest++ = '\0';
        n--;
    }
    return start;
}


task_t *current=NULL;
task_t *idle_task=NULL;
uint32_t next_pid=0;

task_t *task_create(const char *name, task_entry_t entry) {
    if(name==NULL || name[0]=='\0' || entry==NULL) {
        return NULL;
    }
    task_t *task=kmalloc(sizeof(task_t));
    if(task==NULL) return NULL;
    memset(task, 0, sizeof(task_t));
    uint8_t *stack=kmalloc(KERNEL_STACK_SIZE);
    if(stack==NULL) {
        kfree(task);
        return NULL;
    }
    task->pid=next_pid++;
    strncpy(task->name, name, 31);
    task->name[31]='\0';
    task->state=TASK_READY;
    task->kernel_stack_base=(uint32_t)stack;
    task->kernel_stack_size=KERNEL_STACK_SIZE;
    task->page_dir_phys=kernel_page_dir_phys;
    task->ticks_left=DEFAULT_TICKS;
    task->priority=0;
    task->next=NULL;
    task->next_sleep=NULL;
    task->exit_code=0;

    uint32_t top=(uint32_t)stack+KERNEL_STACK_SIZE;
    top&=(~0xf);
    uint32_t *sp=(uint32_t *)top;
    // 压入
    *--sp=(uint32_t)entry;
    *--sp=0x10; // ds
    *--sp=0x10; // es
    *--sp=0x10; // fs
    *--sp=0x10; // gs
    *--sp=0;    // edi
    *--sp=0;    // esi
    *--sp=0;    // ebp
    *--sp=0;    // esp_dummy
    *--sp=0;    // ebx
    *--sp=0;    // edx
    *--sp=0;    // ecx
    *--sp=0;    // eax
    *--sp=0x202;// eflags: IF=1
    task->esp=(uint32_t)sp;

    scheduler_add_task(task);
    return task;
}

int task_destroy(task_t *task) {
    if(task==NULL) return -EINVAL;
    if(task==current) return -EINVAL;
    if(task==idle_task) return -EINVAL;
    if(task->state==TASK_RUNNING) return -EBUSY;
    // TODO: 从就绪队列摘除，并从睡眠队列摘除
    kfree((void *)(task->kernel_stack_base));
    kfree(task);
    return 0;
}

void task_exit(int code) {
    current->state=TASK_ZOMBIE;
    // TODO: 从就绪队列摘除
    current->exit_code=code;
    //schedule(); // 让出CPU
    while(1) hlt();
}

void idle_entry(void) {
    while(1) {
        hlt();
    }
}

task_t *task_get_current(void) {
    return current;
}

void task_set_current(task_t *task) {
    current=task;
}

char *task_state_name(enum task_state state) {
    switch(state) {
        case TASK_RUNNING:
            return "RUNNING";
        case TASK_READY:
            return "READY";
        case TASK_BLOCKED:
            return "BLOCKED";
        case TASK_ZOMBIE:
            return "ZOMBIE";
        default:
            return "UNKNOWN";
    }
}

void task_dump(task_t *task) {
    if(task==NULL) {
        kprintf("task: NULL\n");
        return;
    }
    kprintf("task: pid=%u, name=%s, state=%s, \n", task->pid, task->name, task_state_name(task->state));
    kprintf("      esp=0x%x, kernel_stack_base=0x%x, ticks_left=%u, \n", task->esp, task->kernel_stack_base, task->ticks_left);
    kprintf("      priority=%d\n", task->priority);
    return;
}

void task_init() {
    current=NULL;
    idle_task=NULL;
    next_pid=1;
    idle_task=task_create("idle", idle_entry);
    if(idle_task==NULL) {
        kprintf("FATAL: unable to create task pid=1!\n");
        while(1) hlt();
    }
    current=idle_task;
    idle_task->state=TASK_RUNNING;
    kprintf("task: idle pid=%u\n", idle_task->pid);
}
