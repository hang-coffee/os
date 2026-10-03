//===============================
// layout.h - 内存结构
// hangco, 20261001
//===============================

#ifndef LAYOUT_H
#define LAYOUT_H

#define KERNEL_BASE 0xC0000000
#define USER_BASE 0x00001000
#define USER_TOP 0xbfffffff
#define USER_STACK_TOP 0xbfff0000
#define USER_STACK_MAX 0x00100000
#define USER_STACK_INIT_PAGES 1
#define USER_STACK_LIMIT (USER_STACK_TOP-USER_STACK_MAX)
#define USER_HEAP_START 0x08000000
#define USER_HEAP_MAX 0x40000000
#define USER_CODE_BASE 0x08048000

#endif
