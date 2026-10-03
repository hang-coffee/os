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
// page_fault.c - PF处理
// hangco, 20261002
//===============================

#include "page_fault.h"
#include <stdint.h>
#include <stddef.h>
#include "../include/kprintf.h"
#include "../arch/io.h"
#include "../sched/task.h"
#include "ustack.h"

void isr_handle_pf(idt_regs_t *r) {
    cli();
    uint32_t cr2;
    __asm__ volatile(
        "mov %%cr2, %0"
        : "=r"(cr2)
    );
    uint32_t err=r->err_code;

    if(err&PF_P) {  // 保护违规
        kprintf("\n#PF: eip=0x%08x, cr2=0x%08x, err=0x%x\n", r->eip, cr2, err);
        kprintf("Protection violation at 0x%08x. Killing proc.\n", cr2);
        task_exit(-1);
    }

    // 页不存在
    if(err&PF_US) { // 用户发起
        if(current && cr2>=USER_STACK_LIMIT && cr2<current->stack_top) {
            if(cr2<current->stack_limit) {
                int rc=ustack_expand(current, cr2);
                if(rc==0) return;   // 成功 用户态
            }
        }
        kprintf("\n#PF: eip=0x%08x, cr2=0x%08x, err=0x%x\n", r->eip, cr2, err);
        kprintf("User segfault at 0x%08x, killing process.\n", cr2);
        task_exit(-1);
    }

    kprintf("\n#PF: eip=0x%08x, cr2=0x%08x, err=0x%x\n", r->eip, cr2, err);
    kprintf("Kernel #PF at 0x%08x, halting.\n", cr2);
    for(;;) hlt();
}
