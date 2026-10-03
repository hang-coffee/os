//===============================
// uaccess.h - 用户内存访问校验
// hangco, 20261001
//===============================

#ifndef UACCESS_H
#define UACCESS_H

#include <stdint.h>
#include <stddef.h>

int user_ptr_ok(const void *ptr, size_t len);
int copy_from_user(void *dst, const void *src, size_t len);
int copy_to_user(void *dst, const void *src, size_t len);
int strncpy_from_user(char *dst, const char *src, size_t max);

#endif
