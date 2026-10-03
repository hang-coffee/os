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
// page_fault.h - PF处理
// hangco, 20261002
//===============================

#ifndef PAGE_FAULT_H
#define PAGE_FAULT_H

#include "paging.h"
#include "layout.h"
#include "pmm.h"
#include "ustack.h"
#include "paging.h"
#include "pgdir.h"
#include "../arch/idt.h"

#define PF_P 0x1
#define PF_WR 0x2
#define PF_US 0x4
#define PF_RSVD 0x8

void isr_handle_pf(idt_regs_t *r);

#endif
