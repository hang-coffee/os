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

    kprintf("\n#PF: eip=0x%08x, cr2=0x%08x, err=0x%x\n", r->eip, cr2, err);

    if(err&PF_P) {  // 保护违规
        kprintf("Protection violation at 0x%08x.\n", cr2);
        for(;;) hlt();
    }

    // 页不存在
    if(err&PF_US) { // 用户发起
        if(current && cr2>=USER_STACK_LIMIT && cr2<current->stack_top) {
            if(cr2<current->stack_limit) {
                int rc=ustack_expand(current, cr2);
                kprintf("ustack_expand -> %d\n", rc);
                if(rc==0) return;   // 成功，返回用户态重试
            }
        }
        kprintf("User segfault at 0x%08x, killing process.\n", cr2);
        task_exit(-1);
    }

    kprintf("Kernel #PF at 0x%08x, halting.\n", cr2);
    for(;;) hlt();
}
