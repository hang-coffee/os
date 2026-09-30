//================================
// heap.h - 内核堆
// hangco, 20260930
//================================

#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

#define HEAP_START 0xd0000000
#define HEAP_END 0xd2000000

typedef struct heap_block {     // 本质链表
    uint32_t size;              // 块总大小
    uint32_t free;              // 1=空闲
    struct heap_block *next;
    struct heap_block *prev;
} heap_block_t;

void heap_init(void);
void *kmalloc(uint32_t size);
void kfree(void *ptr);
void *kcalloc(uint32_t nmemb, uint32_t size);
void *krealloc(void *ptr, uint32_t size);
void heap_dump(void);       // 调试函数
int heap_extend(void);

#endif
