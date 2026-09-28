//================================
// pmm.c - 物理内存的分配
// hangco, 20260927
//================================

#include "pmm.h"
#include "mem_map.h"

#include "../include/kprintf.h"

pmm_state_t pmm;

extern uint8_t kernel_end[];
extern uint8_t kernel_start[];

static inline void bitmap_set(uint32_t page) {
    pmm.bitmap[page / 8] |=  (1 << (page % 8));
}
static inline void bitmap_clear(uint32_t page) {
    pmm.bitmap[page / 8] &= ~(1 << (page % 8));
}
static inline int bitmap_test(uint32_t page) {
    return pmm.bitmap[page / 8] & (1 << (page % 8));
}

void pmm_init() {
    // 找到最高可用地址
    uint32_t highest_addr=0;
    for(int i=0; i<mem_region_count; i++) {
        if(mem_map[i].type!=MEM_REGION_TYPE_AVAIL) continue;
        uint32_t end=mem_map[i].base+mem_map[i].length;
        if(end>highest_addr) highest_addr=end;
    }
    pmm.highest_addr=PAGE_ALIGN_UP(highest_addr);
    highest_addr=pmm.highest_addr;
    // 计算总页数和位图大小
    pmm.total_pages=highest_addr/4096;
    uint32_t bitmap_byte=(pmm.total_pages+7)/8;
    uint32_t bitmap_pages=(bitmap_byte+4095)/4096;
    pmm.bitmap_size=bitmap_pages*4096;
    // 计算位图起始地址
    uint32_t bitmap_start=PAGE_ALIGN_UP((uint32_t)kernel_end);
    pmm.bitmap=(uint8_t *)bitmap_start;
    for(uint32_t i=0; i<pmm.bitmap_size; i++) {
        pmm.bitmap[i]=0xff;
    }
    for(int i=0; i<mem_region_count; i++) {
        if(mem_map[i].type!=MEM_REGION_TYPE_AVAIL) continue;
        pmm_mark_range(mem_map[i].base, mem_map[i].length, PMM_FREE);
    }
    pmm_mark_range(0x0, 0x1000, PMM_USED);
    pmm_mark_range(0x10000, 0x1000, PMM_USED);
    pmm_mark_range(0x9FC00, 0x6400, PMM_USED);
    pmm_mark_range(0xA0000, 0x20000, PMM_USED);
    pmm_mark_range((uint32_t)kernel_start, (uint32_t)kernel_end-(uint32_t)kernel_start, PMM_USED);
    pmm_mark_range(bitmap_start, pmm.bitmap_size, PMM_USED);

    for(uint32_t i=0; i<pmm.total_pages; i++) {
        if(bitmap_test(i)) pmm.used_pages++;
        else pmm.free_pages++;
    }
}

void pmm_mark_range(uint32_t base, uint32_t len, uint8_t attr) {
    uint32_t start=base/PAGE_SIZE;
    uint32_t end=(base+len+PAGE_SIZE-1)/PAGE_SIZE;
    if(end>pmm.total_pages) end=pmm.total_pages;
    for(uint32_t i=start; i<end; i++) {
        int was_used=bitmap_test(i);
        if(attr==PMM_USED) {
            if (!was_used) { bitmap_set(i);};
        }
        else {
            if (was_used)  { bitmap_clear(i);}
        }
    }
}

uint32_t pmm_alloc_page(void) {
    for (uint32_t i = 0; i < pmm.total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            pmm.free_pages--;
            pmm.used_pages++;
            return PAGE_TO_ADDR(i);   // 返回物理地址
        }
    }
    return 0;   // 没有空闲页
}

void pmm_free_page(uint32_t addr) {
    uint32_t page = ADDR_TO_PAGE(addr);
    if (page >= pmm.total_pages) return;   // 防御
    if (!bitmap_test(page)) return;        // 双重释放
    bitmap_clear(page);
    pmm.free_pages++;
    pmm.used_pages--;
}

uint32_t pmm_total_pages(void) {
    return pmm.total_pages;
}

uint32_t pmm_free_pages_count(void) {
    return pmm.free_pages;
}

uint32_t pmm_used_pages(void) {
    return pmm.used_pages;
}
