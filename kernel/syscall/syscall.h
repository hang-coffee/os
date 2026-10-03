//===============================
// syscall.h - 系统调用
// hangco, 20261001
//===============================

#ifndef SYSCALL_H
#define SYSCALL_H

#include "../arch/idt.h"

#define SYS_EXIT 1

typedef idt_regs_t syscall_regs_t;
typedef int (*syscall_fn_t)(syscall_regs_t *);

int syscall_handler(syscall_regs_t *r);
void syscall_register(int num, syscall_fn_t fn);
void syscall_init();

extern void syscall_entry();

#endif