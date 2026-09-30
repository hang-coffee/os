//================================
// pmm.h - 物理内存的分配
// hangco, 20260927
//================================

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PAGE_MASK (PAGE_SIZE-1)

#define PAGE_ALIGN_UP(x) (((x)+PAGE_MASK)&~PAGE_MASK)
#define PAGE_ALIGN_DOWN(x) (((x)&~PAGE_MASK))
#define ADDR_TO_PAGE(x) ((x)>>PAGE_SHIFT)
#define PAGE_TO_ADDR(x) ((x)<<PAGE_SHIFT)

#define PMM_FREE 0
#define PMM_USED 1

typedef struct pmm_state {
    uint8_t *bitmap;                // 位图地址，此处是线性地址
    uint32_t bitmap_size;           // 位图大小，BYTE
    uint32_t total_pages;           // 页数
    uint32_t free_pages;            // 空闲页数
    uint32_t used_pages;            // 使用的页数
    uint32_t highest_addr;          // 最高内存地址
    uint32_t rsvd_pages;            // 保留页数
    uint32_t bitmap_phys;           // 位图的物理地址
} pmm_state_t;

extern pmm_state_t pmm;

void pmm_init();
void pmm_set_physmap();
void pmm_mark_range(uint32_t base, uint32_t len, uint8_t attr);
uint32_t pmm_alloc_page(void);
void pmm_free_page(uint32_t addr);
uint32_t pmm_total_pages(void);
uint32_t pmm_free_pages_count(void);
uint32_t pmm_used_pages(void);

#endif
