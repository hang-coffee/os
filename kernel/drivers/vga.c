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
// vga.c - VGA文本模式驱动
// hangco, 20260926
//================================

#include "vga.h"
#include "../arch/io.h"
#include <stdint.h>

volatile uint16_t *vga_text=(volatile uint16_t *)0xc00b8000;

void vga_putc(vga_context_t *vga, uint8_t c, uint8_t color) {
	uint8_t x_now, y_now;
	x_now=vga->xpos; y_now=vga->ypos;
	if(c=='\n') {
		y_now++;
		x_now=0;
	} else if(c=='\t') {
		x_now=(x_now+8)&~7;
	} else if(c=='\b') {
		if(x_now!=0) {
			x_now--;
		}
	} else if(c=='\r') {
		x_now=0;
	} else {
		vga_text[pos2addr(vga->xpos, vga->ypos)]=((color<<8)+c);
		x_now++;
	}

	if(x_now>vga->xpos_max) {
		x_now=0;
		y_now++;
	}
	if(y_now>vga->ypos_max) {
		vga_scroll(vga);
		y_now=vga->ypos_max;
	}
	vga->xpos=x_now;
	vga->ypos=y_now;
	vga_update_pos(vga);
	return;
}

void vga_scroll(vga_context_t *vga) {
	for(int y=0; y<vga->ypos_max; y++) {
		for(int x=0; x<=vga->xpos_max; x++) {
			vga_text[pos2addr(x, y)]=vga_text[pos2addr(x, y+1)];
		}
	}
	for(int x=0; x<=vga->xpos_max; x++) {
		vga_text[pos2addr(x, vga->ypos_max)]=0x0700+' ';
	}
}

void vga_update_pos(vga_context_t *vga) {
	outb(0x3d4, 0x0f);
	outb(0x3d5, pos2addr(vga->xpos, vga->ypos)&0xff);
	outb(0x3d4, 0x0e);
	outb(0x3d5, (pos2addr(vga->xpos, vga->ypos)>>8)&0xff);
	return;
}

void vga_puts(vga_context_t *vga, char *s, uint8_t color) {
	int i=0;
	while(s[i]!='\0') {
		vga_putc(vga, s[i], color);
		i++;
	}
	return;
}

void vga_clear(vga_context_t *vga) {
	for(int x=0; x<=vga->xpos_max; x++) {
		for(int y=0; y<=vga->ypos_max; y++) {
			vga_text[pos2addr(x, y)]=0x0700+' ';
		}
	}
	vga->xpos=0; vga->ypos=0;
	vga_update_pos(vga);
	return;
}

vga_context_t vga_get_info() {
	vga_context_t ret;
	uint16_t addr;
	outb(0x3d4, 0x0e);
	addr=inb(0x3d5)<<8;
	outb(0x3d4, 0x0f);
	addr|=inb(0x3d5);
	ret.xpos_max=79;
	ret.ypos_max=24;
	ret.xpos=addr%80;
	ret.ypos=addr/80;
	return ret;
}
