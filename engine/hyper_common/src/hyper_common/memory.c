/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/memory.h"

#include "hyper_common/assertion.h"

#include <stdlib.h>
#include <string.h>

#if HYPER_DEBUG
struct hyper_allocation_header
{
	size_t size;
};

static struct hyper_allocation_debug_info *s_allocation_debug_info;
#endif

void *hyper_allocate(size_t size)
{
	hyper_assert$(size != 0);

#if HYPER_DEBUG
	uint8_t *ptr = malloc(size + sizeof(struct hyper_allocation_header));
	if (ptr == NULL)
	{
		return NULL;
	}

	struct hyper_allocation_header *allocation_header =
		(struct hyper_allocation_header *) ptr;
	ptr += sizeof(struct hyper_allocation_header);

	allocation_header->size = size;

	s_allocation_debug_info->total_bytes += size;
	s_allocation_debug_info->current_bytes += size;
	s_allocation_debug_info->total_allocs++;

	return ptr;
#else
	return malloc(size);
#endif
}

void *hyper_callocate(size_t size)
{
	void *ptr = hyper_allocate(size);
	if (ptr == NULL)
	{
		return NULL;
	}

	memset(ptr, 0x00, size);

	return ptr;
}

void hyper_deallocate(void *ptr)
{
	hyper_assert$(ptr != NULL);

#if HYPER_DEBUG
	uint8_t *real_ptr = (uint8_t *) ptr - sizeof(struct hyper_allocation_header);
	struct hyper_allocation_header *allocation_header =
		(struct hyper_allocation_header *) real_ptr;

	s_allocation_debug_info->current_bytes -= allocation_header->size;
	s_allocation_debug_info->total_frees++;

	free(real_ptr);
#else
	free(ptr);
#endif
}

void hyper_set_allocation_debug_info(
	struct hyper_allocation_debug_info *allocation_debug_info)
{
#if HYPER_DEBUG
	s_allocation_debug_info = allocation_debug_info;
#endif
}

uint64_t hyper_get_total_allocs()
{
#if HYPER_DEBUG
	return s_allocation_debug_info->total_allocs;
#else
	return 0;
#endif
}

uint64_t hyper_get_total_frees()
{
#if HYPER_DEBUG
	return s_allocation_debug_info->total_frees;
#else
	return 0;
#endif
}

uint64_t hyper_get_total_bytes()
{
#if HYPER_DEBUG
	return s_allocation_debug_info->total_bytes;
#else
	return 0;
#endif
}

uint64_t hyper_get_current_bytes()
{
#if HYPER_DEBUG
	return s_allocation_debug_info->current_bytes;
#else
	return 0;
#endif
}
