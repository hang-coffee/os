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

//===============================
// pgdir.c - 用户态页目录管理
// hangco, 20261001
//===============================

#include "pgdir.h"
#include "pmm.h"
#include "paging.h"
#include "../include/errno.h"
#include <stdint.h>
#include <stddef.h>

extern void *memset(void *s, int c, uint32_t n);

uint32_t *pgdir_create(uint32_t *out_phys) {  // 分配一页物理页作为页目录，返回线性地址
    uint32_t phys=pmm_alloc_page();
    if(phys==0) return NULL;
    uint32_t *virt=(uint32_t *)PHYS_TO_VIRT(phys);
    memset(virt, 0, 4096);
    uint32_t *kernel_pd=(uint32_t *)PHYS_TO_VIRT(kernel_page_dir_phys);
    for(int i=768; i<1024; i++) {
        virt[i]=kernel_pd[i];
    }
    (*out_phys)=phys;
    return virt;
}

void pgdir_destroy(uint32_t *pgdir) {    // 取消页目录分配
    if(pgdir==NULL) return;
    for(int i=0; i<768; i++) {
        uint32_t pde=pgdir[i];
        if(!(pde&PAGE_PRESENT)) continue;
        uint32_t pt_phys=pde&(~0xfff);
        uint32_t *pt=(uint32_t *)(PHYS_TO_VIRT(pt_phys));
        for(int j=0; j<1024; j++) {
            uint32_t pte=pt[j];
            if(!(pte&PAGE_PRESENT)) continue;
            uint32_t page_phys=pte&(~0xfff);
            pmm_free_page(page_phys);
        }
        pmm_free_page(pt_phys);
    }
    pmm_free_page(page_get_phys((uint32_t)pgdir));
    return;
}

void pgdir_switch(uint32_t *pgdir) { // 切换页目录，如果为NULL切换为内核的
    if(pgdir==NULL) paging_load_pdir(kernel_page_dir_phys);
    else paging_load_pdir(page_get_phys((uint32_t)pgdir));
    return;
}

int map_user_page(uint32_t *pgdir, uint32_t vaddr, uint32_t paddr, uint32_t flags) {
    if(pgdir==NULL) return -EINVAL;
    if(vaddr>=KERNEL_BASE) return -EINVAL;
    uint32_t pdi=vaddr>>22;
    uint32_t pti=(vaddr>>12)&0x3ff;
    uint32_t pt_phys, *pt;
    if(!(pgdir[pdi]&PAGE_PRESENT)) {
        pt_phys=pmm_alloc_page();
        if(pt_phys==0) return -ENOMEM;
        pt=(uint32_t *)(PHYS_TO_VIRT(pt_phys));
        memset(pt, 0, 4096);
        pgdir[pdi]=pt_phys|PAGE_PRESENT|PAGE_RW|PAGE_USER;
    } else {
        pt_phys=pgdir[pdi]&(~0xfff);
        pt=(uint32_t *)(PHYS_TO_VIRT(pt_phys));
    }
    pt[pti]=(paddr&(~0xfff))|(flags&0xfff)|PAGE_PRESENT;
    paging_invlpg(vaddr);
    return 0;
}

int unmap_user_page(uint32_t *pgdir, uint32_t vaddr) {
    if(pgdir==NULL) return 0;
    if(vaddr>=KERNEL_BASE) return -EINVAL;
    uint32_t pdi=vaddr>>22;
    uint32_t pti=(vaddr>>12)&0x3ff;
    if(!(pgdir[pdi]&PAGE_PRESENT)) {
        return -EINVAL;
    }
    uint32_t pt_phys=pgdir[pdi]&(~0xfff);
    uint32_t *pt=(uint32_t *)(PHYS_TO_VIRT(pt_phys));
    if(!(pt[pti]&PAGE_PRESENT)) return -EINVAL;
    pt[pti]=0;
    paging_invlpg(vaddr);
    return 0;
}

uint32_t get_user_physical(uint32_t *pgdir, uint32_t vaddr) {
    if(vaddr>=KERNEL_BASE) return 0;
    uint32_t pdi=vaddr>>22;
    uint32_t pti=(vaddr>>12)&0x3ff;
    if(!(pgdir[pdi]&PAGE_PRESENT)) {
        return 0;
    }
    uint32_t pt_phys=pgdir[pdi]&(~0xfff);
    uint32_t *pt=(uint32_t *)(PHYS_TO_VIRT(pt_phys));
    if(!(pt[pti]&PAGE_PRESENT)) return 0;
    return (pt[pti]&(~0xfff))|(vaddr&0xfff);
}
