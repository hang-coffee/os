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

//================================
// io.h - IO函数
// hangco, 20260926
//================================

#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t data) {
	__asm__ __volatile__(
		"outb %b0, %w1"
		:
		: "a"(data), "d"(port)
	);
}

static inline void outw(uint16_t port, uint16_t data) {
	__asm__ __volatile__(
		"outw %w0, %w1"
		:
		: "a"(data), "d"(port)
	);
}

static inline void outd(uint16_t port, uint32_t data) {
	__asm__ __volatile__(
		"outd %d0, %w1"
		:
		: "a"(data), "d"(port)
	);
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	__asm__ __volatile__(
		"inb %w1, %b0"
		: "=a"(ret)
		: "d"(port)
	);
	return ret;
}

static inline uint16_t inw(uint16_t port) {
	uint16_t ret;
	__asm__ __volatile__(
		"inw %w1, %w0"
		: "=a"(ret)
		: "d"(port)
	);
	return ret;
}

static inline uint32_t ind(uint16_t port) {
	uint32_t ret;
	__asm__ __volatile__(
		"inb %w1, %d0"
		: "=a"(ret)
		: "d"(port)
	);
	return ret;
}

static inline void cli(void) {
	__asm__ __volatile__(
		"cli"
		::
	);
	return;
}

static inline void sti(void) {
	__asm__ __volatile__(
		"sti"
		::
	);
	return;
}

static inline void hlt(void) {
	__asm__ __volatile__(
		"hlt"
		::
	);
	return;
}

#define IO_INT(n) __asm__ __volatile__("int $" #n ::: "memory")

#endif

