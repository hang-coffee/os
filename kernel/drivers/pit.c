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
// pit.c - 定时器设定
// hangco, 20261001
//===============================

#include "pit.h"
#include <stdint.h>
#include "../arch/irq.h"
#include "../include/kprintf.h"
#include "../arch/io.h"
#include "../sched/sched.h"
#include "../arch/pic.h"

static volatile uint64_t pit_ticks;
static uint32_t pit_freq;

#define PIT_CH0 0x40
#define PIT_CMD 0x43
#define PIT_BASE_FREQ 1193182

void pit_init(uint32_t freq) {
    pit_freq=freq;
    pit_ticks=0;
    uint32_t divisor=PIT_BASE_FREQ/freq;
    if(divisor<1 || divisor>65535) {
        kprintf("pit: illegal frequency 0x%x when initializing\n     divisor=%u", freq, divisor);
        return;
    }
    outb(PIT_CMD, 0x36);
    outb(PIT_CH0, divisor&0xff);
    outb(PIT_CH0, (divisor>>8)&0xff);
    irq_register_handler(0, pit_handler);
    return;
}

void pit_handler(irq_regs_t *r) {
    pit_ticks++;
    pic_eoi(0);
    scheduler_tick(r);
}

uint64_t pit_get_ticks() {
    return pit_ticks;
}

uint64_t pit_get_freq() {
    return pit_freq;
}
