/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_math/hyper_math.h"

#include "hyper_common/debug.h"
#include "hyper_common/memory.h"
#include "hyper_common/prerequisites.h"

enum hyper_result hyper_module_start(void *memory_info)
{
	hyper_unused_debug_variable$(memory_info);

#if HYPER_DEBUG
	hyper_set_memory_info(memory_info);
#endif
	return HYPER_RESULT_SUCCESS;
}
