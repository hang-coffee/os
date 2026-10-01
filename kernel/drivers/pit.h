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
// pit.h - 定时器设定
// hangco, 20261001
//===============================

#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include "../arch/irq.h"

void pit_init(uint32_t freq);
void pit_handler(irq_regs_t *r);
uint64_t pit_get_ticks();
uint64_t pit_get_freq();

#endif
