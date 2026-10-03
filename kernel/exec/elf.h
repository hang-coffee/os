//===============================
// elf.h - ELF加载器
// hangco, 20261003
//===============================

#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include "elf_def.h"

int elf_validate(const uint8_t *data, uint32_t size);
int elf_load_segment(uint32_t *pgdir, const uint8_t *data, uint32_t file_size, const Elf32_Phdr *ph);
int elf_load(uint32_t *pgdir, const uint8_t *data, uint32_t size, uint32_t *entry_out);
void elf_unload(uint32_t *pgdir, const uint8_t *data, uint32_t size);

#endif
