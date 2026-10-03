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
// paging.c - 分页
// hangco, 20260930
//================================

#include "paging.h"
#include "pmm.h"
#include <stdint.h>
#include "../include/errno.h"
#include "../include/kprintf.h"

uint32_t kernel_page_dir_phys=0;
uint32_t *kernel_page_dir=0;

void paging_init(void) {
    kernel_page_dir_phys=pmm_alloc_page();
    if(kernel_page_dir_phys==0) {
        kprintf("paging: cannot allocate page dir\n");
        return;
    }
    uint32_t *pd=(uint32_t *)kernel_page_dir_phys;
    for(int i=0; i<1024; i++) pd[i]=0;
    // 计算所需的PDE数量
    uint32_t map_limit=(pmm.highest_addr+0x3fffff)&(~0x3fffff);
    uint32_t num_pde=map_limit/0x400000;
    if(num_pde>256) num_pde=256;
    // 将内核物理地址map到高地址
    uint32_t pd_idx_base=KERNEL_BASE>>22;
    for(uint32_t i=0; i<num_pde; i++) {
        uint32_t pt_phys=pmm_alloc_page();
        if(pt_phys==0) {
            kprintf("paging: cannot allocate page table %u\n", i);
            return;
        }
        uint32_t *pt=(uint32_t *)pt_phys;
        for(int j=0; j<1024; j++) {
            uint32_t phys=i*0x400000+j*0x1000;
            pt[j]=phys|PAGE_PRESENT|PAGE_RW;
        }
        pd[pd_idx_base+i]=pt_phys|PAGE_PRESENT|PAGE_RW;
    }
    // 撤销低地址恒等映射
    for(uint32_t i=0; i<4; i++) {
        pd[i]=0;
    }
    kernel_page_dir=pd;
    kernel_page_dir_phys=(uint32_t)pd;
    paging_load_pdir(kernel_page_dir_phys);
    paging_enable();
    pmm_set_physmap();
    kprintf("paging: enabled, physmap @ 0x%08x, %u PDEs\n", KERNEL_BASE, num_pde);
    return;
}

int page_map(uint32_t virt, uint32_t phys, uint32_t flags) {
    // 检查数据
    if((virt&0xfff)||(phys&0xfff)) {
        return -EINVAL;
    }
    // 计算页目录索引和页表索引
    uint32_t pd_idx=virt>>22, pt_idx=(virt>>12)&0x3ff;  // pt_idx，页表索引，是线性地址的低10位
    uint32_t pde=((uint32_t *)(PHYS_TO_VIRT(kernel_page_dir_phys)))[pd_idx];      // 在#PF要求开新页时，这行会带来新的#PF 
    uint32_t pt_phys;   // 页表的地址
    if(pde&1) { // P位为1，存在这样一个页表
        pt_phys=pde&(~0xfff);    // 高20位为地址
    } else {
        pt_phys=pmm_alloc_page();   // 分配一个新的pmm页，作为页表 TODO：内核使用线性地址
        if(pt_phys==0) return -ENOMEM;      // 内存不足，无法分配
        for(int i=0; i<1024; i++) {
            ((uint32_t *)(PHYS_TO_VIRT(pt_phys)))[i]=0;     // 清零这个页表
        }
        // 填写页目录，这里的权限U/S继承自flags的U/S
        ((uint32_t *)PHYS_TO_VIRT(kernel_page_dir_phys))[pd_idx]=(pt_phys)|PAGE_PRESENT|PAGE_RW|PAGE_USER|(flags&4);  // TODO:来自PDE的权限控制
    }
    // 获取页表项
    uint32_t *pt=(uint32_t *)(PHYS_TO_VIRT(pt_phys&(~0xfff)));
    uint32_t pte=pt[pt_idx];
    if(pte&1) {     // 已经存在了
        return -EEXIST;
    }
    pte=phys|flags|PAGE_PRESENT;
    pt[pt_idx]=pte;
    paging_invlpg(virt);
    return 0;
}

int page_unmap(uint32_t virt) {
    if(virt&0xfff) return -EINVAL;
    uint32_t pd_idx=virt>>22, pt_idx=(virt>>12)&0x3ff;
    uint32_t *pd=(uint32_t *)PHYS_TO_VIRT(kernel_page_dir_phys);
    uint32_t pde=pd[pd_idx];
    if(!(pde&0x1)) return -EINVAL;
    uint32_t pt_phys=pde&(~0xfff);
    uint32_t *pt=PHYS_TO_VIRT(pt_phys);
    uint32_t pte=pt[pt_idx];
    if(!(pte&0x1)) return -EINVAL;
    pt[pt_idx]=0;
    paging_invlpg(virt);
    return 0;
}

uint32_t page_get_phys(uint32_t virt) {
    uint32_t offset=virt&0xfff;
    virt&=(~0xfff);
    uint32_t pd_idx=virt>>22, pt_idx=(virt>>12)&0x3ff;
    uint32_t *pd=(uint32_t *)PHYS_TO_VIRT(kernel_page_dir_phys);
    uint32_t pde=pd[pd_idx];
    if(!(pde&0x1)) return 0;
    uint32_t pt_phys=pde&(~0xfff);
    uint32_t *pt=PHYS_TO_VIRT(pt_phys);
    uint32_t pte=pt[pt_idx];
    if(!(pte&0x1)) return 0;
    uint32_t page_base=pte&(~0xfff);
    return page_base+offset;
}
