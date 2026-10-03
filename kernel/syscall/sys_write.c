//===============================
// sys_write.c - sys_write函数
// hangco, 20261003
//===============================

#include "sys_write.h"
#include <stddef.h>
#include "../include/errno.h"
#include "../mm/uaccess.h"
#include "../drivers/vga.h"
#include "../drivers/serial.h"
#include "../include/kprintf.h"

int sys_write(syscall_regs_t *r) {
    int fd=(int)r->ebx;
    const char *buf=(const char *)r->ecx;
    uint32_t count=r->edx;
        kprintf("[sys_write] enter: fd=%u buf=%p count=%u\n",
            r->ebx, r->ecx, r->edx);
    if(count==0) return 0;
    if(buf==NULL) return -EFAULT;
    if(!user_ptr_ok(buf, count)) return -EFAULT;
    vga_context_t vga=vga_get_info();
    serial_context_t serial={.base_port=0x3f8};
    if(fd==1 || fd==2) {
        for(uint32_t i=0; i<count; i++) {
            char c=buf[i];
            vga_putc(&vga, c, 0x07);
            serial_putc(&serial, c);
        }
        return (int)count;
    }
    return -EBADF;
}
