/*
 * This file is part of Congestus.
 * Copyright (C) 2026 hangco
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

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
