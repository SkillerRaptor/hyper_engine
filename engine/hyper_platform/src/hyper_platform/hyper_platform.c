/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/hyper_platform.h"

#include "hyper_common/memory.h"

enum hyper_result hyper_module_start(void *allocation_debug_info)
{
#if HYPER_DEBUG
	hyper_set_allocation_debug_info(allocation_debug_info);
#endif
	return HYPER_RESULT_SUCCESS;
}