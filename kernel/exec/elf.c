//===============================
// elf.c - ELF32加载器
// hangco, 20261003
//===============================

#include "elf.h"
#include "elf_def.h"
#include <stdint.h>
#include <stddef.h>
#include "../include/errno.h"
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "../mm/pgdir.h"
#include "../mm/layout.h"

extern void *memcpy(void *dest, const void *src, uint32_t n);
extern void *memset(void *s, int c, uint32_t n);

int elf_validate(const uint8_t *data, uint32_t size) {
    if(data==NULL||size<sizeof(Elf32_Ehdr)) return -ENOEXEC;
    if(data[0]!=0x7f||data[1]!='E'||data[2]!='L'||data[3]!='F') return -ENOEXEC;
    Elf32_Ehdr *ehdr=(Elf32_Ehdr *)data;
    if(ehdr->e_ident[EI_CLASS]!=ELFCLASS32) return -ENOEXEC;
    if(ehdr->e_ident[EI_DATA]!=ELFDATA2LSB) return -ENOEXEC;
    if(ehdr->e_ident[EI_VERSION]!=EV_CURRENT) return -ENOEXEC;
    if(ehdr->e_type!=ET_EXEC) return -ENOEXEC;
    if(ehdr->e_machine!=EM_386) return -ENOEXEC;
    if(ehdr->e_version!=EV_CURRENT) return -ENOEXEC;
    if(ehdr->e_phentsize!=sizeof(Elf32_Phdr)) return -ENOEXEC;
    if(ehdr->e_phnum==0) return -ENOEXEC;
    if(ehdr->e_phoff+ehdr->e_phnum*ehdr->e_phentsize>size) return -ENOEXEC;
    if(ehdr->e_entry>=KERNEL_BASE) return -ENOEXEC;
    return 0;
}

int elf_load_segment(uint32_t *pgdir, const uint8_t *data, uint32_t file_size, const Elf32_Phdr *ph) {
    if(pgdir==NULL||data==NULL||ph==NULL) return -EINVAL;
    uint32_t vaddr=ph->p_vaddr;
    uint32_t memsz=ph->p_memsz;
    uint32_t filesz=ph->p_filesz;
    uint32_t offset=ph->p_offset;
    if(filesz>memsz) return -ENOEXEC;
    if(offset+filesz>file_size) return -ENOEXEC;
    if(vaddr>=KERNEL_BASE) return -ENOEXEC;
    if(vaddr+memsz>KERNEL_BASE) return -ENOEXEC;
    if(memsz==0) return 0;
    uint32_t vaddr_page=vaddr&(~0xfff);
    uint32_t page_delta=vaddr-vaddr_page;
    uint32_t total_bytes=memsz+page_delta;
    uint32_t page_count=(total_bytes+0xfff)/0x1000;
    uint32_t flags=PAGE_PRESENT|PAGE_USER;
    if(ph->p_flags&PF_W) flags|=PAGE_RW;
    for(uint32_t i=0; i<page_count; i++) {
        uint32_t cur_vaddr=vaddr_page+i*0x1000;
        uint32_t phys=pmm_alloc_page();
        if(phys==0) goto fail;
        memset(PHYS_TO_VIRT(phys), 0, 4096);
        int32_t page_in_seg=(int32_t)(i*0x1000)-(int32_t)page_delta;
        if(page_in_seg<(int32_t)filesz) {
            uint32_t copy_start, copy_end, copy_bytes, src_off, dst_off;
            if(page_in_seg<0) {
                copy_start=0;
                dst_off=(uint32_t)(-page_in_seg);
            } else {
                copy_start=(uint32_t)page_in_seg;
                dst_off=0;
            }
            copy_end=copy_start+0x1000;
            if(copy_end>filesz) copy_end=filesz;
            copy_bytes=(copy_end>copy_start)?(copy_end-copy_start):0;
            if(copy_bytes>0) {
                src_off=offset+copy_start;
                memcpy((uint8_t *)PHYS_TO_VIRT(phys)+dst_off, data+src_off, copy_bytes);
            }
        }
        if(map_user_page(pgdir, cur_vaddr, phys, flags)) {
            pmm_free_page(phys);
            goto fail;
        }
    }
    return 0;
fail:
    for(uint32_t j=0; j<page_count; j++) {
        uint32_t v=vaddr_page+j*0x1000;
        uint32_t p=get_user_physical(pgdir, v);
        if(p) {
            unmap_user_page(pgdir, v);
            pmm_free_page(p&(~0xfff));
        }
    }
    return -ENOMEM;
}

int elf_load(uint32_t *pgdir, const uint8_t *data, uint32_t size, uint32_t *entry_out) {
    if(pgdir==NULL||data==NULL||entry_out==NULL) return -EINVAL;
    int ret=elf_validate(data, size);
    if(ret) return ret;
    Elf32_Ehdr *ehdr=(Elf32_Ehdr *)data;
    uint32_t loaded=0;
    for(uint32_t i=0; i<ehdr->e_phnum; i++) {
        Elf32_Phdr *ph=(Elf32_Phdr *)(data+ehdr->e_phoff+i*sizeof(Elf32_Phdr));
        if(ph->p_type!=PT_LOAD) continue;
        if(ph->p_memsz==0) continue;
        ret=elf_load_segment(pgdir, data, size, ph);
        if(ret) {
            elf_unload(pgdir, data, size);
            return ret;
        }
        loaded++;
    }
    if(loaded==0) return -ENOEXEC;
    *entry_out=ehdr->e_entry;
    return 0;
}

void elf_unload(uint32_t *pgdir, const uint8_t *data, uint32_t size) {
    if(pgdir==NULL||data==NULL) return;
    if(size<sizeof(Elf32_Ehdr)) return;
    if(data[0]!=0x7f||data[1]!='E'||data[2]!='L'||data[3]!='F') return;
    Elf32_Ehdr *ehdr=(Elf32_Ehdr *)data;
    if(ehdr->e_phentsize!=sizeof(Elf32_Phdr)) return;
    if(ehdr->e_phoff+ehdr->e_phnum*ehdr->e_phentsize>size) return;
    for(uint32_t i=0; i<ehdr->e_phnum; i++) {
        Elf32_Phdr *ph=(Elf32_Phdr *)(data+ehdr->e_phoff+i*sizeof(Elf32_Phdr));
        if(ph->p_type!=PT_LOAD) continue;
        if(ph->p_memsz==0) continue;
        uint32_t vaddr_page=ph->p_vaddr&(~0xfff);
        uint32_t page_delta=ph->p_vaddr-vaddr_page;
        uint32_t total_bytes=ph->p_memsz+page_delta;
        uint32_t page_count=(total_bytes+0xfff)/0x1000;
        for(uint32_t j=0; j<page_count; j++) {
            uint32_t v=vaddr_page+j*0x1000;
            uint32_t p=get_user_physical(pgdir, v);
            if(p) {
                unmap_user_page(pgdir, v);
                pmm_free_page(p&(~0xfff));
            }
        }
    }
    return;
}