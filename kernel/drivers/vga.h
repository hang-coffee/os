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
// vga.h - VGA文本模式驱动
// hangco, 20260926
//================================

#ifndef VGA_H
#define VGA_H

#include <stdint.h>

static inline int pos2addr(uint8_t x, uint8_t y) {
	return (y*80)+x;
}

typedef struct {
	// 认为VGA文本模式是坐标轴的第四象限
	uint8_t xpos;				// 光标横坐标
	uint8_t ypos;				// 光标纵坐标
	uint8_t xpos_max;			// 最大横坐标，79
	uint8_t ypos_max;			// 最大纵坐标，24
} vga_context_t;

void vga_putc(vga_context_t *vga, uint8_t c, uint8_t color);
void vga_scroll(vga_context_t *vga); // 纯粹滚屏，不更改光标信息
void vga_update_pos(vga_context_t *vga);
void vga_puts(vga_context_t *vga, char *s, uint8_t color);
void vga_clear(vga_context_t *vga); // 清屏并重置光标
vga_context_t vga_get_info();

#endif
