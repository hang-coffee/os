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
