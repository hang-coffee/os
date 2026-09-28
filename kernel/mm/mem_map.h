//================================
// mem_map.h - 解析内存
// hangco, 20260927
//================================

#ifndef MEM_MAP_H
#define MEM_MAP_H

#include <stdint.h>

enum mem_region_type {
    MEM_REGION_TYPE_AVAIL=1,
    MEM_REGION_TYPE_RSVD=2,
    MEM_REGION_TYPE_ACPI_RECL=3,
    MEM_REGION_TYPE_ACPI_NVS=4,
    MEM_REGION_TYPE_BAD=5,
};

typedef struct mem_region {
	uint32_t base;
	uint32_t length;
	enum mem_region_type type;
} mem_region_t;

#define MAX_MEM_REGIONS 32
extern mem_region_t mem_map[MAX_MEM_REGIONS];
extern int mem_region_count;
extern uint32_t mem_total_avail;

void mmap_init(uint32_t info);
void mmap_add(uint32_t addr, uint32_t len, uint32_t type);
void mmap_print_avail();

#endif
