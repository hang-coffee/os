//================================
// kprintf.h - 内核printf输出
// hangco, 20260926
//================================

#ifndef KPRINTF_H
#define KPRINTF_H

#include <stdarg.h>
#include <stddef.h>
#include "../drivers/vga.h"
#include "../drivers/serial.h"

// 返回值：成功为输出字符数，失败为-1

typedef struct {
    vga_context_t vga;
    serial_context_t serial;
}putc_context_t;

typedef void (*putc_fn)(void *ctx, char c); // 输出回调
int kvprintf(putc_fn putc, void *ctx, const char *fmt, va_list ap);
int kprintf(const char *fmt, ...);
int kpanic(const char *fmt, ...);

#endif
