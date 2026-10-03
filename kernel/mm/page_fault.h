//===============================
// page_fault.h - PF处理
// hangco, 20261002
//===============================

#ifndef PAGE_FAULT_H
#define PAGE_FAULT_H

#include "paging.h"
#include "layout.h"
#include "pmm.h"
#include "ustack.h"
#include "paging.h"
#include "pgdir.h"
#include "../arch/idt.h"

#define PF_P 0x1
#define PF_WR 0x2
#define PF_US 0x4
#define PF_RSVD 0x8

void isr_handle_pf(idt_regs_t *r);

#endif
