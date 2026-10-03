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
// pgdir.h - 用户态页目录管理
// hangco, 20261001
//===============================

#ifndef PGDIR_H
#define PGDIR_H

#include <stdint.h>

uint32_t *pgdir_create(uint32_t *out_phys);  // 分配一页物理页作为页目录，返回线性地址
void pgdir_destroy(uint32_t *pgdir);    // 取消页目录分配
void pgdir_switch(uint32_t *pgdir); // 切换页目录，如果为NULL切换为内核的

int map_user_page(uint32_t *pgdir, uint32_t vaddr, uint32_t paddr, uint32_t flags);
int unmap_user_page(uint32_t *pgdir, uint32_t vaddr);
uint32_t get_user_physical(uint32_t *pgdir, uint32_t vaddr);

#endif
