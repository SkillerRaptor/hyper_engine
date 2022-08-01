/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/library.h"

#include "hyper_common/assertion.h"

#include <stdlib.h>

#if HYPER_PLATFORM_WINDOWS
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#elif HYPER_PLATFORM_LINUX
#	include <dlfcn.h>
#endif

enum hyper_result hyper_library_open(
	struct hyper_library *library,
	const char *library_path)
{
	HYPER_ASSERT(library != NULL);
	HYPER_ASSERT(library_path != NULL);

#if HYPER_PLATFORM_WINDOWS
	library->handle = LoadLibrary(library_path);
#elif HYPER_PLATFORM_LINUX
	library->handle = dlopen(library_path, RTLD_NOW | RTLD_LOCAL);
#endif

	if (library->handle == NULL)
	{
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_library_close(struct hyper_library *library)
{
	HYPER_ASSERT(library != NULL);

#if HYPER_PLATFORM_WINDOWS
	FreeLibrary(library->handle);
#elif HYPER_PLATFORM_LINUX
	dlclose(library->handle);
#endif
}

void *hyper_library_get(struct hyper_library *library, const char *proc_name)
{
	HYPER_ASSERT(library != NULL);
	HYPER_ASSERT(proc_name != NULL);

#if HYPER_PLATFORM_WINDOWS
	return (void *) GetProcAddress(library->handle, proc_name);
#elif HYPER_PLATFORM_LINUX
	return (void *) dlsym(library, proc_name);
#endif
}
