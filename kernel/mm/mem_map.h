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
