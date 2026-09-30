//================================
// paging.h - 分页
// hangco, 20260930
//================================

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// 页表信息定义
#define PAGE_PRESENT 0x01
#define PAGE_RW 0x02
#define PAGE_USER 0x04
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY 0x40

#define KERNEL_BASE 0xc0000000
#define PHYS_TO_VIRT(p) ((void *)((uint32_t)(p) + KERNEL_BASE))
#define VIRT_TO_PHYS(v) ((uint32_t)(v) - KERNEL_BASE)

void paging_init(void);
int page_map(uint32_t virt, uint32_t phys, uint32_t flags); // 将virt线性地址和phys物理地址关联并建立页表项，返回0为正常，非0为异常
int page_unmap(uint32_t virt); // 只清空P位，返回值为0则正常，负值不正常
uint32_t page_get_phys(uint32_t virt); // 得到物理地址，0为没有映射

extern uint32_t kernel_page_dir_phys;
extern uint32_t *kernel_page_dir;

extern void paging_load_pdir(uint32_t phys);
extern void paging_enable(void);
extern void paging_invlpg(uint32_t virt);

#endif
