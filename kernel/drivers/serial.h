//================================
// serial.h - 串口驱动
// hangco, 20260926
//================================

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

typedef struct {
    uint16_t base_port;
} serial_context_t;

void serial_init(serial_context_t *serial, uint16_t base);
void serial_wait_empty(serial_context_t *serial);
void serial_wait_data_ready(serial_context_t *serial);
void serial_putc(serial_context_t *serial, uint8_t c);
void serial_puts(serial_context_t *serial, char *s);
// TODO: 还有一个ISR，现阶段不需要写

#endif
