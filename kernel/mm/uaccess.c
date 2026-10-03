//===============================
// uaccess.c - 用户内存访问校验
// hangco, 20261001
//===============================

#include "uaccess.h"
#include <stdint.h>
#include <stddef.h>
#include "../include/errno.h"

int user_ptr_ok(const void *ptr, size_t len) {   // 返回1为合法，0为非法
    uint32_t p=(uint32_t)ptr;
    uint32_t end=p+len;
    if(end<p) return 0;                 // 溢出
    if(p<0x1000) return 0;              // 空指针页
    if(end>0xc0000000) return 0;        // 越入内核
    return 1;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned int *d = (unsigned int *)dest;
    const unsigned int *s = (const unsigned int *)src;
    size_t count = n / 4;
    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }
    // 处理剩余字节
    unsigned char *d_byte = (unsigned char *)(d + count);
    const unsigned char *s_byte = (const unsigned char *)(s + count);
    for (size_t i = 0; i < n % 4; i++) {
        d_byte[i] = s_byte[i];
    }
    return dest;
}

int copy_from_user(void *dst, const void *src, size_t len) {
    if(!user_ptr_ok(src, len)) return -EFAULT;   // src来自用户
    memcpy(dst, src, len);
    return 0;
}

int copy_to_user(void *dst, const void *src, size_t len) {
    if(!user_ptr_ok(dst, len)) return -EFAULT;   // dst指向用户
    memcpy(dst, src, len);
    return 0;
}

int strncpy_from_user(char *dst, const char *src, size_t max) {
    if(dst==NULL||src==NULL||max==0) {
        return -EINVAL;
    }
    for(size_t i=0;i<max;i++) {
        if(!user_ptr_ok(src+i, 1)) {
            dst[i]='\0';
            return -EFAULT;
        }
        dst[i]=src[i];
        if(dst[i]=='\0') {
            return (int)i;
        }
    }
    dst[max-1]='\0';
    return -ENAMETOOLONG;
}