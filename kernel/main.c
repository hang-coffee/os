//================================
// main.c - 内核的main
// hangco, 20260926
//================================

#include "include/kprintf.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/io.h"
#include "arch/pic.h"
#include "arch/irq.h"
#include "drivers/pit.h"
#include "mm/mem_map.h"
#include "mm/multiboot.h"
#include "mm/pmm.h"
#include "mm/paging.h"
#include "mm/heap.h"

#include <stdint.h>

void heap_test_basic(void) {
    kprintf("=== heap test: basic ===\n");
    void *a = kmalloc(100);
    void *b = kmalloc(200);
    void *c = kmalloc(50);
    if (!a || !b || !c) {
        kprintf("  FAIL: alloc returned NULL\n");
        return;
    }
    // 写入并读出
    for (int i = 0; i < 100; i++) ((uint8_t *)a)[i] = i & 0xFF;
    for (int i = 0; i < 100; i++) {
        if (((uint8_t *)a)[i] != (i & 0xFF)) {
            kprintf("  FAIL: data mismatch at %d\n", i);
            return;
        }
    }
    // 地址不重叠
    if ((uint32_t)a == (uint32_t)b || (uint32_t)b == (uint32_t)c) {
        kprintf("  FAIL: overlapping alloc\n");
        return;
    }
    kfree(a); kfree(b); kfree(c);
    kprintf("  PASS\n");
    heap_dump();
}
#define N 256
void heap_test_fragment(void) {
    kprintf("=== heap test: fragment ===\n");
    static void *p[N];
    for (int i = 0; i < N; i++) {
        p[i] = kmalloc(64);
        if (!p[i]) { kprintf("  FAIL: alloc %d NULL\n", i); return; }
        *(uint32_t *)p[i] = i;   // 写标记
    }
    // 隔一个释放
    for (int i = 0; i < N; i += 2) kfree(p[i]);
    // 验证剩下的一半内容完好
    for (int i = 1; i < N; i += 2) {
        if (*(uint32_t *)p[i] != (uint32_t)i) {
            kprintf("  FAIL: data corrupted at %d\n", i);
            return;
        }
    }
    // 全部释放
    for (int i = 1; i < N; i += 2) kfree(p[i]);
    kprintf("  PASS\n");
    heap_dump();   // 应只剩 1-2 个大空闲块
}
#define RAND_N 512
static uint32_t rng_state = 0x12345678;
static uint32_t rng(void) {
    rng_state = rng_state * 1103515245 + 12345;
    return rng_state >> 16;
}

void heap_test_random(void) {
    kprintf("=== heap test: random ===\n");
    static void *p[RAND_N];
    static uint32_t sz[RAND_N];
    for (int i = 0; i < RAND_N; i++) { p[i] = 0; sz[i] = 0; }

    int allocs = 0, frees = 0;
    for (int round = 0; round < 2000; round++) {
        uint32_t idx = rng() % RAND_N;
        if (p[idx] == 0) {
            // 分配 1 - 512 字节
            uint32_t size = (rng() % 512) + 1;
            p[idx] = kmalloc(size);
            if (!p[idx]) {
                kprintf("  FAIL: alloc size=%u round=%d\n", size, round);
                return;
            }
            sz[idx] = size;
            // 写入一个可校验的 pattern
            uint8_t *q = (uint8_t *)p[idx];
            for (uint32_t k = 0; k < size; k++) q[k] = (uint8_t)(idx ^ k);
            allocs++;
        } else {
            // 校验 pattern 完好再释放
            uint8_t *q = (uint8_t *)p[idx];
            for (uint32_t k = 0; k < sz[idx]; k++) {
                if (q[k] != (uint8_t)(idx ^ k)) {
                    kprintf("  FAIL: corruption idx=%u k=%u round=%d\n",
                            idx, k, round);
                    return;
                }
            }
            kfree(p[idx]);
            p[idx] = 0;
            frees++;
        }
    }
    // 收尾
    for (int i = 0; i < RAND_N; i++) if (p[i]) kfree(p[i]);
    kprintf("  PASS (allocs=%d frees=%d)\n", allocs, frees);
    heap_dump();
}

void heap_test_extend(void) {
    kprintf("=== heap test: extend ===\n");
    // 假设初始堆只有几页，先要 1 MiB
    void *big = kmalloc(1024 * 1024);
    if (!big) {
        kprintf("  FAIL: cannot alloc 1 MiB\n");
        return;
    }
    // 写入首尾各一段，验证跨页正常
    uint8_t *q = (uint8_t *)big;
    for (int i = 0; i < 256; i++) q[i] = (uint8_t)i;
    for (int i = 0; i < 256; i++) {
        if (q[i] != (uint8_t)i) {
            kprintf("  FAIL: head corrupt at %d\n", i);
            return;
        }
    }
    for (int i = 1024 * 1024 - 256; i < 1024 * 1024; i++) q[i] = (uint8_t)i;
    for (int i = 1024 * 1024 - 256; i < 1024 * 1024; i++) {
        if (q[i] != (uint8_t)i) {
            kprintf("  FAIL: tail corrupt at %d\n", i);
            return;
        }
    }
    kfree(big);
    kprintf("  PASS\n");
    heap_dump();
}

void heap_test_edge(void) {
    kprintf("=== heap test: edge ===\n");
    // NULL 安全
    kfree(NULL);

    // 野指针（应被防御拦截，不崩）
    // 注意：如果 kfree 没有范围检查，这一步会崩，注释掉
    kfree((void *)0xDEADBEEF);

    // 0 字节分配
    void *z = kmalloc(0);
    if (z) kfree(z);   // 若返回非 NULL，必须能 free

    // 超大分配（超过堆上限，应返回 NULL）
    void *huge = kmalloc(64 * 1024 * 1024);
    if (huge) {
        kprintf("  FAIL: 64 MiB alloc should fail\n");
        kfree(huge);
        return;
    }

    kprintf("  PASS\n");
}

void kernel_main(uint32_t magic, uint32_t info) {
	gdt_init();
	idt_init();
	cli();
	if(magic!=MULTIBOOT_BOOTLOADER_MAGIC) {
		kprintf("Bad bootloader magic: expected 0x%x got 0x%x\n", magic, info);
		return;
	}
	mmap_init(info);
	pmm_init();
	mmap_print_avail();
	paging_init();
	kprintf("Total pages: %d\nFree: %d / Used: %d\n", pmm_total_pages(), pmm_free_pages_count(), pmm_used_pages());
	heap_init();

    pic_init();
    irq_init();

    pit_init(100);
    pic_unmask(0);
    sti();

    kprintf("before sleep\n");
    sleep_ms(1000);
    kprintf("after sleep\n");

	while(1);
}

