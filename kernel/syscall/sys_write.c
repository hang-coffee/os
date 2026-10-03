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
