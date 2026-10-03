//===============================
// ustack.c - 用户栈
// hangco, 20261002
//===============================

#include "ustack.h"
#include <stdint.h>
#include <stddef.h>
#include "../include/errno.h"
#include "paging.h"
#include "pmm.h"
#include "pgdir.h"

extern void *memset(void *s, int t, uint32_t n);

int ustack_setup(task_t *t) {    // 设置用户栈
    if(t->pgdir==NULL) return -EINVAL;
    if(t->stack_top!=0) return -EINVAL;
    _Static_assert(USER_STACK_INIT_PAGES<=USER_STACK_MAX/4096, "Unreasonable USER_STACK_INIT_PAGES & USER_STACK_MAX");
    uint32_t stack_bottom=USER_STACK_TOP-USER_STACK_INIT_PAGES*4096;
    for(uint32_t vaddr=stack_bottom; vaddr<USER_STACK_TOP; vaddr+=4096) {
        uint32_t phys=pmm_alloc_page();
        if(phys==0) {
            for(uint32_t j=stack_bottom; j<vaddr; j+=4096) {
                uint32_t p=get_user_physical(t->pgdir, j);
                unmap_user_page(t->pgdir, j);
                if(p) pmm_free_page(p&(~0xfff));
            }
            return -ENOMEM;
        }
        memset(PHYS_TO_VIRT(phys), 0, 4096);
        if(map_user_page(t->pgdir, vaddr, phys, PAGE_USER|PAGE_RW)) {
            for(uint32_t j=stack_bottom; j<vaddr; j+=4096) {
                uint32_t p=get_user_physical(t->pgdir, j);
                unmap_user_page(t->pgdir, j);
                if(p) pmm_free_page(p&(~0xfff));
            }
            return -ENOMEM;
        }
    }
    t->stack_top=USER_STACK_TOP;
    t->stack_limit=stack_bottom;
    return 0;
}

int ustack_expand(task_t *t, uint32_t addr) {    // 从addr开始扩展用户栈
    uint32_t page=addr&(~0xfff);
    if(page>=t->stack_limit) return -EINVAL;
    if(page<USER_STACK_LIMIT) return -ENOMEM;
    uint32_t phys=pmm_alloc_page();
    if(phys==0) return -ENOMEM;
    memset(PHYS_TO_VIRT(phys), 0, 4096);
    int ret=map_user_page(t->pgdir, page, phys, PAGE_USER|PAGE_RW);
    if(ret) {
        pmm_free_page(phys);
        return -ENOMEM;
    }
    t->stack_limit=page;
    return 0;
}