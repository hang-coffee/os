//===============================
// lifecycle.h - 生命周期类
// hangco, 20261001
//===============================

#ifndef LIFECYCLE_H
#define LIFECYCLE_H

#include "syscall.h"

int sys_exit(syscall_regs_t *r);    // ebx=status

#endif
