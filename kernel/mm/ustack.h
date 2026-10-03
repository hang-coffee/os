//===============================
// ustack.h - 用户栈
// hangco, 20261002
//===============================

#ifndef USTACK_H
#define USTACK_H

#include "layout.h"

#define STACK_PAGE_OF(addr) ((addr)&(~0xfff))
#define IS_IN_STACK_RANGE(t, addr) \
    ((addr)>=USER_STACK_LIMIT && (addr)<(t)->stack_top)
#define STACK_IS_EXPANDING(t, addr) \
    (IS_IN_STACK_RANGE(t, addr)&&(addr)<(t)->stack_limit)

#include "../sched/task.h"

int ustack_setup(task_t *t);    // 设置用户栈
int ustack_expand(task_t *t, uint32_t addr);    // 从addr开始扩展用户栈

#endif
