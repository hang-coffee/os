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
void sleep_ms(uint32_t ms);

#endif
