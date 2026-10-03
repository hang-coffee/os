//===============================
// sys_write.h - sys_write函数
// hangco, 20261003
//===============================

#ifndef SYS_WRITE_H
#define SYS_WRITE_H

#define SYS_WRITE 4

#include "syscall.h"

int sys_write(syscall_regs_t *r);

#endif
