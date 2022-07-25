/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

struct hyper_memory_info
{
	uint64_t total_allocs;
	uint64_t total_frees;
	uint64_t total_bytes;
	uint64_t current_bytes;
};

void hyper_set_memory_info(struct hyper_memory_info *memory_info);

void *hyper_allocate(size_t size);
void *hyper_callocate(size_t size);
void hyper_deallocate(void *ptr);
