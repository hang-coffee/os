//================================
// kprintf.c - 内核printf输出
// hangco, 20260926
//================================

#include "../include/kprintf.h"
#include "../drivers/vga.h"
#include "../drivers/serial.h"

static void console_putc(void *ctx, char c) {
    vga_putc(&((putc_context_t *)ctx)->vga, c, 0x0f);
    serial_putc(&((putc_context_t *)ctx)->serial, c);
}

int kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    putc_context_t ctx={.serial={.base_port=0x3f8}, .vga=vga_get_info()};
    int n = kvprintf(console_putc, &ctx, fmt, ap);
    va_end(ap);
    return n;
}

int kpanic(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    putc_context_t ctx={.serial={.base_port=0x3f8}, .vga=vga_get_info()};
    int n = kvprintf(console_putc, &ctx, fmt, ap);
    va_end(ap);
    for (;;) __asm__ volatile ("cli; hlt");
    return n;
}
