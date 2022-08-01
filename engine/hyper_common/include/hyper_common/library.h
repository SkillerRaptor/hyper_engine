/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/compiler.h"
#include "hyper_common/platform.h"
#include "hyper_common/result.h"

#if HYPER_PLATFORM_WINDOWS
#	define HYPER_SHARED_PREFIX ""
#	define HYPER_SHARED_EXTENSION ".dll"
#
#	if HYPER_SYMBOLS_EXPORT
#		if HYPER_COMPILER_GCC
#			define HYPER_API __attribute__((dllexport))
#		else
#			define HYPER_API __declspec(dllexport)
#		endif
#	else
#		if HYPER_COMPILER_GCC
#			define HYPER_API __attribute__((dllimport))
#		else
#			define HYPER_API __declspec(dllimport)
#		endif
#	endif
#elif HYPER_PLATFORM_LINUX
#	define HYPER_SHARED_PREFIX "lib"
#	define HYPER_SHARED_EXTENSION ".so"
#
#	define HYPER_API __attribute__((visibility("default")))
#endif

struct hyper_library
{
	void *handle;
};

enum hyper_result hyper_library_open(
	struct hyper_library *library,
	const char *library_path);
void hyper_library_close(struct hyper_library *library);

void *hyper_library_get(struct hyper_library *library, const char *proc_name);
