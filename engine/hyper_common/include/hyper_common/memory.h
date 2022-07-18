/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/debug.h"
#include "hyper_common/library.h"

#include <stddef.h>
#include <stdint.h>

struct hyper_allocation_debug_info
{
	uint64_t total_allocs;
	uint64_t total_frees;
	uint64_t total_bytes;
	uint64_t current_bytes;
};

void *hyper_allocate(size_t size);
void *hyper_callocate(size_t size);
void hyper_deallocate(void *ptr);

void hyper_set_allocation_debug_info(
	struct hyper_allocation_debug_info *allocation_debug_info);

uint64_t hyper_get_total_allocs();
uint64_t hyper_get_total_frees();
uint64_t hyper_get_total_bytes();
uint64_t hyper_get_current_bytes();
