//===============================
// trampoline.c - ring3蹦床
// hangco, 20261003
//===============================

#include "trampoline.h"
#include "task.h"

void user_trampoline(void) {
    uint32_t user_eip=current->user_entry;
    uint32_t user_esp=current->user_esp;
    __asm__ volatile (
        ".intel_syntax noprefix\n"
        "cli\n"
        "mov ax, 0x23\n"    // 用户数据段
        "mov ds, ax\n"
        "mov es, ax\n"
        "mov fs, ax\n"
        "mov gs, ax\n"
        "push 0x23\n"       // ss
        "push %0\n"         // esp
        "push 0x202\n"      // eflags: if=1
        "push 0x1b\n"       // cs
        "push %1\n"         // eip
        "iretd\n"
        ".att_syntax"
        :
        : "r"(user_esp), "r"(user_eip)
        : "eax", "memory"
    );
    __builtin_unreachable();
}
