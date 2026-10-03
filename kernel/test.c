/*
 * 纯用户程序调用测试
 * 只验证：创建用户任务 → Ring 3 → 系统调用 → 退出
 */

#include "sched/task.h"
#include "sched/sched.h"
#include "mm/pmm.h"
#include "mm/paging.h"
#include "mm/pgdir.h"
#include "mm/ustack.h"
#include "mm/layout.h"
#include "sched/trampoline.h"
#include "include/kprintf.h"

#include <stdint.h>

/* 用户程序二进制符号 */
extern const uint8_t _binary_build_user_test_user_bin_start[];
extern const uint8_t _binary_build_user_test_user_bin_end[];

extern void *memcpy(void *dst, const void *src, unsigned int n);
extern void *memset(void *dst, int c, unsigned int n);

#define PMAP(phys)  ((void *)((uint32_t)(phys) + KERNEL_BASE))

/* 内核任务栈布局：与 task_create 保持一致 */
static void build_kernel_stack(task_t *t, uint32_t entry) {
    uint32_t *sp = (uint32_t *)(t->kernel_stack_base + KERNEL_STACK_SIZE);
    sp = (uint32_t *)((uint32_t)sp & ~0xF);

    *--sp = entry;    /* ret 目标 */
    *--sp = 0x10;     /* ds */
    *--sp = 0x10;     /* es */
    *--sp = 0x10;     /* fs */
    *--sp = 0x10;     /* gs */
    *--sp = 0;        /* edi */
    *--sp = 0;        /* esi */
    *--sp = 0;        /* ebp */
    *--sp = 0;        /* esp_dummy */
    *--sp = 0;        /* ebx */
    *--sp = 0;        /* edx */
    *--sp = 0;        /* ecx */
    *--sp = 0;        /* eax */
    *--sp = 0x202;    /* eflags: IF=1 */
    t->esp = (uint32_t)sp;
}

/* 把 flat binary 映射到用户地址空间 */
static int load_user_code(uint32_t *pgdir,
                          const uint8_t *code, uint32_t size) {
    uint32_t off = 0;
    while (off < size) {
        uint32_t phys = pmm_alloc_page();
        if (!phys) return -1;

        void *page = PMAP(phys);
        memset(page, 0, 4096);
        uint32_t chunk = (size - off) < 4096 ? (size - off) : 4096;
        memcpy(page, code + off, chunk);

        if (map_user_page(pgdir, USER_CODE_BASE + off, phys,
                          PAGE_PRESENT | PAGE_RW | PAGE_USER) != 0) {
            pmm_free_page(phys);
            return -1;
        }
        off += 4096;
    }
    return 0;
}

/*
 * 创建一个用户任务并加入就绪队列。
 * 返回 task 指针，失败返回 NULL。
 */
task_t *spawn_user(const char *name) {
    const uint8_t *code = _binary_build_user_test_user_bin_start;
    uint32_t size = (uint32_t)
        (_binary_build_user_test_user_bin_end
       - _binary_build_user_test_user_bin_start);

    if (size == 0) {
        kprintf("[spawn] user binary empty\n");
        return NULL;
    }

    /* 1. 创建任务结构（entry=NULL，不自动入队） */
    task_t *t = task_create(name, NULL);
    if (!t) {
        kprintf("[spawn] task_create failed\n");
        return NULL;
    }

    /* 2. 创建独立页目录 */
    t->pgdir = pgdir_create(&t->page_dir_phys);
    if (!t->pgdir) {
        kprintf("[spawn] pgdir_create failed\n");
        task_destroy(t);
        return NULL;
    }

    /* 3. 映射用户代码 */
    if (load_user_code(t->pgdir, code, size) != 0) {
        kprintf("[spawn] load_user_code failed\n");
        pgdir_destroy(t->pgdir);
        t->pgdir = NULL;
        task_destroy(t);
        return NULL;
    }

    /* 4. 建立用户栈 */
    if (ustack_setup(t) != 0) {
        kprintf("[spawn] ustack_setup failed\n");
        pgdir_destroy(t->pgdir);
        t->pgdir = NULL;
        task_destroy(t);
        return NULL;
    }

    /* 5. 用户入口与栈顶 */
    t->user_entry = USER_CODE_BASE;
    t->user_esp   = USER_STACK_TOP - 16;

    /* 6. 构造内核栈：ret -> user_trampoline */
    build_kernel_stack(t, (uint32_t)user_trampoline);

    /* 7. 入队 */
    scheduler_add_task(t);

    kprintf("[spawn] pid=%u, entry=0x%x, esp=0x%x\n",
            t->pid, t->user_entry, t->user_esp);
    return t;
}

/* 测试入口 */
void test_usermode_only(void) {
    kprintf("\n=== usermode-only test ===\n");

    task_t *t = spawn_user("hello");
    if (!t) {
        kprintf("test failed: cannot spawn user task\n");
        return;
    }

    kprintf("waiting for user output...\n");
    /* 后续由时钟中断驱动调度，用户任务会自动运行 */
}