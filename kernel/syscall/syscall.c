//===============================
// syscall.c - 系统调用
// hangco, 20261001
//===============================

#include "syscall.h"
#include "../include/errno.h"
#include <stddef.h>
#include <stdint.h>
#include "../include/kprintf.h"

#include "lifecycle.h"
#include "sys_write.h"

static syscall_fn_t syscall_table[256];

int syscall_handler(syscall_regs_t *r) {
    kprintf("syscall: num=%u, ebx=%x, ecx=%x, edx=%x, cs=%x\n",
        r->eax, r->ebx, r->ecx, r->edx, r->cs);
    uint32_t num=r->eax;
    if(num>=256 || syscall_table[num]==NULL) return -ENOSYS;
    return syscall_table[num](r);
}

void syscall_register(int num, syscall_fn_t fn) {
    if(num>=0 && num<256) {
        syscall_table[num]=fn;
    }
}

void syscall_init() {
    idt_set_gate(0x80, (uint32_t)syscall_entry, 0x08, 0xEE);
    syscall_register(SYS_EXIT, sys_exit);
    syscall_register(SYS_WRITE, sys_write);
}