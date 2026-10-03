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
        kprintf("Page Fault:\n");
        kprintf("The instruction at 0x%08x referenced memory at 0x%08x.\n", r->eip, cr2);
        kprintf("The memory could not be %s.\n", (err&PF_WR)?("written"):("read"));
        // TODO: 发送SIGSEGV
        cli();
        hlt();
    } else {
        // 页不存在
        if(err&PF_US) { // 用户发起
            if(cr2>=USER_STACK_LIMIT && cr2<current->stack_top) {
                // 栈
                if(cr2<current->stack_limit) {
                    ustack_expand(current, cr2);
                }
            }
        }
    }
}
