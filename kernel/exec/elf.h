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
