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
// pic.h - PIC(8259A)的驱动
// hangco, 20261001
//================================

#ifndef PIC_H
#define PIC_H

#define PIC_MASTER_LOW 0x20
#define PIC_MASTER_HIGH 0x21
#define PIC_SLAVE_LOW 0xa0
#define PIC_SLAVE_HIGH 0xa1

#include <stdint.h>

void pic_init(void);
void pic_mask(uint8_t irq);     // 屏蔽特定IRQ
void pic_unmask(uint8_t irq);   // 不屏蔽特定IRQ
void pic_eoi(uint8_t irq);      // 发送中断完成

uint8_t pic_get_irr(void);      // 获得中断请求
uint8_t pic_get_isr(void);      // 获得中断服务

uint16_t pic_get_mask(void);    // 获得掩码，高位是SLAVE，低位是MASTER
void pic_set_mask(uint8_t master, uint8_t slave);

#endif
