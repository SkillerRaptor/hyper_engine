/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/platform.h"

#include <string.h>

#if HYPER_PLATFORM_WINDOWS
#	define hyper_strip_file_name$(file_name) \
		(strrchr(file_name, '\\') ? strrchr(file_name, '\\') + 1 : file_name)
#elif HYPER_PLATFORM_LINUX
#	define hyper_strip_file_name$(file_name) \
		(strrchr(file_name, '/') ? strrchr(file_name, '/') + 1 : file_name)
#else
#	define hyper_strip_file_name$(file_name) ((void) 0)
#endif
