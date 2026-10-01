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

#include "sched.h"
#include <stdint.h>
#include <stddef.h>
#include "../include/errno.h"
#include "../drivers/pit.h"
#include "task.h"

static task_t *ready_head=NULL;
static task_t *ready_tail=NULL;
static uint32_t ready_count=0;

static task_t *sleep_head=NULL;
static task_t *sleep_tail=NULL;
static uint32_t sleep_count=0;

void scheduler_init() {
    ready_head=NULL;
    ready_tail=NULL;
    ready_count=0;
    sleep_head=NULL;
    sleep_tail=NULL;
    sleep_count=0;
    return;
}

int scheduler_add_task(task_t *task) {
    if(task==NULL) return -EINVAL;
    if(task->state!=TASK_READY) task->state=TASK_READY;
    task->next=NULL;
    if(ready_count==0) {
        ready_head=ready_tail=task;
    } else {
        ready_tail->next=task;
        ready_tail=task;
    }
    ready_count++;
    return 0;
}

int scheduler_remove_task(task_t *task) {
    if(task==NULL) return -EINVAL;
    if(ready_head==NULL) return -EINVAL;
    if(task==ready_head) {
        ready_head=task->next;
        if(ready_head==NULL) ready_tail=NULL;   // 队列空了
    } else {
        task_t *prev=ready_head;
        while(prev->next!=task && prev->next) {
            prev=prev->next;
        }
        if(prev->next!=task) return -ESRCH;
        prev->next=task->next;
        if(task==ready_tail) ready_tail=prev;
    }
    task->next=NULL;
    ready_count--;
    return 0;
}

task_t *scheduler_pick_next(void) {
    if(ready_count==0) return idle_task;
    return ready_head;
}

void switch_to(task_t *next) {
    if(next==NULL) return;
    if(next==current) return;
    task_t *prev=current;
    current=next;
    next->state=TASK_RUNNING;
    if(prev->state==TASK_RUNNING) prev->state=TASK_READY;
    switch_context(&prev->esp, next->esp);
}

void schedule() {   // 任务主动调用，而不是ISR
    if(ready_count==0) return;
    if(ready_count==1) return;
    scheduler_remove_task(current);
    current->state=TASK_READY;
    scheduler_add_task(current);
    task_t *next=ready_head;
    switch_to(next);
    return;
}

void scheduler_tick(irq_regs_t *r) {
    if(current==NULL) return;
    // 唤醒到期的睡眠任务
    while(sleep_head!=NULL && sleep_head->wakeup_tick<=pit_get_ticks()) {
        task_t *wake=sleep_head;
        sleep_head=wake->next_sleep;
        if(sleep_head==NULL) sleep_tail=NULL;
        wake->next_sleep=NULL;
        sleep_count--;
        wake->state=TASK_READY;
        scheduler_add_task(wake);
    }
    // 时间片
    current->ticks_left--;
    if(current->ticks_left>0) return;
    current->ticks_left=DEFAULT_TICKS;
    scheduler_remove_task(current);
    current->state=TASK_READY;
    scheduler_add_task(current);
    task_t *next=ready_head;
    if(next==current) return;
    switch_to(next);
    return;
}

void sleep_queue_add(task_t *task) {
    if(task==NULL) return;
    task->next_sleep=NULL;
    if(sleep_head==NULL) {
        sleep_head=sleep_tail=task;
    } else if(task->wakeup_tick<sleep_head->wakeup_tick) {
        task->next_sleep=sleep_head;
        sleep_head=task;
    } else {
        task_t *prev=sleep_head;
        while(prev->next_sleep!=NULL && prev->next_sleep->wakeup_tick<=task->wakeup_tick) {
            prev=prev->next_sleep;
        }
        task->next_sleep=prev->next_sleep;
        prev->next_sleep=task;
        if(task->next_sleep==NULL) sleep_tail=task;
    }
    sleep_count++;
    return;
}

int sleep_queue_remove(task_t *task) {
    if(task==NULL) return -EINVAL;
    if(sleep_head==NULL) return -EINVAL;
    if(task==sleep_head) {
        sleep_head=task->next_sleep;
        if(sleep_head==NULL) sleep_tail=NULL;
    } else {
        task_t *prev=sleep_head;
        while(prev->next_sleep!=task && prev->next_sleep) {
            prev=prev->next_sleep;
        }
        if(prev->next_sleep!=task) return -ESRCH;
        prev->next_sleep=task->next_sleep;
        if(task==sleep_tail) sleep_tail=prev;
    }
    task->next_sleep=NULL;
    sleep_count--;
    return 0;
}

void task_sleep(task_t *task) {
    if(task==NULL) return;
    task->state=TASK_BLOCKED;
    scheduler_remove_task(task);
    sleep_queue_add(task);
    task_t *next=scheduler_pick_next();
    switch_to(next);
    return;
}

void sleep_ms(uint32_t ms) {
    if(ms==0) return;
    if(current==NULL) return;
    current->wakeup_tick=pit_get_ticks()+(uint64_t)ms*pit_get_freq()/1000;
    task_sleep(current);
    return;
}

void task_block(int reason) {
    if(current==NULL) return;
    current->state=TASK_BLOCKED;
    current->block_reason=reason;
    scheduler_remove_task(current);
    task_t *next=scheduler_pick_next();
    switch_to(next);
    return;
}

void task_unblock(task_t *task) {
    if(task==NULL) return;
    if(task->state!=TASK_BLOCKED) return;
    sleep_queue_remove(task);   // 不在睡眠队列时返回 -ESRCH，无害
    task->state=TASK_READY;
    scheduler_add_task(task);
    return;
}
