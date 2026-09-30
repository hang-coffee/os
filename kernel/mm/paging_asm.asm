;================================
; paging_asm.asm - 分页（汇编）
;================================

section .text
bits 32
global paging_load_pdir
global paging_enable
global paging_invlpg
    paging_load_pdir:
    ; void paging_load_pdir(uint32_t phys)
    mov eax, [esp+4]    ; phys
    mov cr3, eax
    ret

    paging_enable:
    ; void paging_enable(void)
    push eax
    mov eax, cr0
    or eax, 0x80010000
    mov cr0, eax
    pop eax
    ret

    paging_invlpg:
    ; void paging_invlpg(uint32_t virt)
    mov eax, [esp+4]
    invlpg [eax]
    ret
