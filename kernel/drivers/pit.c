//===============================
// pit.c - 定时器设定
// hangco, 20261001
//===============================

#include "pit.h"
#include <stdint.h>
#include "../arch/irq.h"
#include "../include/kprintf.h"
#include "../arch/io.h"

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
        kprintf("pit: illegal frequency 0x%x when initializing\n", freq);
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
    if(pit_ticks%100==0)
        kprintf("tick @ %llu: one second\n", pit_ticks);
    // TODO: 完成多任务中的scheduler_tick()
}

uint64_t pit_get_ticks() {
    return pit_ticks;
}

void sleep_ms(uint32_t ms) {
    uint64_t target=pit_ticks+ms*pit_freq/1000;
    while(pit_ticks<target) hlt();
    return;
}
