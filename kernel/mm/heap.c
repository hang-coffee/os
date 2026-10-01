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
// heap.c - 内核堆
// hangco, 20260930
//================================

#include "heap.h"
#include <stddef.h>
#include <stdint.h>

#include "pmm.h"
#include "paging.h"
#include "../include/kprintf.h"
#include "../include/errno.h"

#define HEAP_ALIGN 8
#define MIN_SPLIT 16
#define ALIGN_UP(x) (((x)+(HEAP_ALIGN-1))&~(HEAP_ALIGN-1))
heap_block_t *block_list=NULL;
uint32_t heap_start=HEAP_START, heap_end=HEAP_START;

void heap_init(void) {
    for(int i=0; i<16; i++) {
        uint32_t phys=pmm_alloc_page();
        if(phys==0) {
            kprintf("Kernel heap initialization err\n");
            return;
        }
        page_map(heap_end, phys, PAGE_PRESENT|PAGE_RW);
        heap_end+=PAGE_SIZE;
    }
    heap_block_t *first=(heap_block_t *)heap_start;
    first->size=heap_end-heap_start;
    first->free=1;
    first->next=NULL;
    first->prev=NULL;
    block_list=first;
}

void *kmalloc(uint32_t size) {
    if(size==0) return NULL;
    size=ALIGN_UP(size);
    uint32_t need=size+sizeof(heap_block_t);
    again:
    heap_block_t *block=block_list;
    while(block!=NULL) {    // 遍历链表
        if(block->free && block->size>=need) {  // 只要满足需要
            uint32_t rest=block->size-need;
            if(rest>=sizeof(heap_block_t)+MIN_SPLIT) {  // 处理分裂
                heap_block_t *new=(heap_block_t *)((uint8_t *)block+need);
                new->size=rest;
                new->free=1;
                new->prev=block;
                new->next=block->next;
                if(block->next) block->next->prev=new;
                block->size=need;
                block->next=new;
            }
            block->free=0;
            return (void *)(block+1);
        }
        block=block->next;
    }
    // 如果执行到这里，那么说明现在的堆过小了。
    if(heap_extend()==0) goto again;
    else return NULL;
}

int heap_extend() {
    if(heap_end+PAGE_SIZE>HEAP_END) return -ENOMEM;
    uint32_t phys=pmm_alloc_page();
    if(phys==0) return -ENOMEM;
    int ret=page_map(heap_end, phys, PAGE_PRESENT|PAGE_RW);
    if(ret) {
        pmm_free_page(phys);
        return -ENOMEM;
    }
    heap_block_t *last=block_list;
    while(last->next!=NULL) {
        last=last->next;
    }
    if(last->free==1 && (uint32_t)last+last->size==heap_end) {
        last->size+=PAGE_SIZE;
    } else {
        heap_block_t *nb=((heap_block_t *)heap_end);
        nb->size=PAGE_SIZE;
        nb->free=1;
        nb->prev=last;
        nb->next=NULL;
        last->next=nb;
    }
    heap_end+=PAGE_SIZE;
    return 0;
}

void kfree(void *ptr) {
    if(ptr==NULL) return;
    heap_block_t *block=(heap_block_t *)ptr-1;
    uint32_t addr=(uint32_t)block;
    if(heap_start<=addr && addr<heap_end && block->free==0 && block->size>=sizeof(heap_block_t) && addr+block->size<=heap_end) {
        block->free=1;
        heap_block_t *next=block->next;
        if(next!=NULL && next->free==1) {
            block->size+=next->size;
            block->next=next->next;
            if(next->next!=NULL) {
                next->next->prev=block;
            }
        }
        heap_block_t *prev=block->prev;
        if(prev!=NULL && prev->free==1) {
            prev->size+=block->size;
            prev->next=block->next;
            if(block->next!=NULL) {
                block->next->prev=prev;
            }
        }
        // TODO: 尾部收缩
    }
}

void heap_dump(void) {
    kprintf("===== HEAP DUMP =====\n");
    kprintf("range: 0x%08x - 0x%08x (%u B)\n",
            heap_start, heap_end, heap_end - heap_start);

    struct heap_block *b = block_list;
    int i = 0;
    while (b) {
        kprintf("  #%d [0x%08x] size=%u %s\n",
                i++, (uint32_t)b, b->size, b->free ? "free" : "used");
        b = b->next;
    }
    kprintf("=====================\n");
}

void *kcalloc(uint32_t nmemb, uint32_t size) {
    if(nmemb!=0 && (nmemb*size)/nmemb!=size) return NULL;
    uint32_t total=nmemb*size;
    void *ptr=kmalloc(total);
    if(ptr) {
        for(int i=0; i<total; i++) ((uint8_t *)(ptr))[i]=0;
    }
    return 0;
}

void *krealloc(void *ptr, uint32_t size) {
    if(ptr==NULL) return kmalloc(size);
    if(size==0) {
        kfree(ptr);
        return NULL;
    }
    return NULL;        // TODO: 完成完整的这个函数
}
