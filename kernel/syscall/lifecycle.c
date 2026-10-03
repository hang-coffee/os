//===============================
// lifecycle.c - 生命周期类
// hangco, 20261001
//===============================

#include "lifecycle.h"
#include "../sched/task.h"
#include "../drivers/pit.h"
#include "../mm/paging.h"
#include "../mm/pmm.h"
#include <stddef.h>
#include <stdint.h>

int sys_exit(syscall_regs_t *r) {    // ebx=status
    int status=(int)(r->ebx & 0xff);
    task_exit(status);
    __builtin_unreachable();
    return 0;
}
