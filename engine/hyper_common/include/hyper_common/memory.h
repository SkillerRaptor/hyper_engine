/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

void *hyper_allocate(size_t size);
void *hyper_callocate(size_t size);
void hyper_deallocate(void *ptr);

uint64_t hyper_get_total_allocs();
uint64_t hyper_get_total_frees();
uint64_t hyper_get_total_bytes();
uint64_t hyper_get_current_bytes();
