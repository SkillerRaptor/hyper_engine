/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/compiler.h"
#include "hyper_common/platform.h"

#if HYPER_PLATFORM_WINDOWS
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#
#	define HYPER_SHARED_PREFIX ""
#	define HYPER_SHARED_EXTENSION ".dll"
#
#	define hyper_open_library$(library) LoadLibrary(library)
#	define hyper_close_library$(library) FreeLibrary(library)
#	define hyper_get_proc_address$(library, proc_name) \
		GetProcAddress(library, proc_name)
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
#	include <dlfcn.h>
#
#	define HYPER_SHARED_PREFIX "lib"
#	define HYPER_SHARED_EXTENSION ".so"
#
#	define hyper_open_library$(library) dlopen(library, RTLD_NOW | RTLD_LOCAL)
#	define hyper_close_library$(library) dlclose(library)
#	define hyper_get_proc_address$(library, proc_name) dlsym(library, proc_name)
#
#	define HYPER_API __attribute__((visibility("default")))
#else
#	define HYPER_SHARED_PREFIX ""
#	define HYPER_SHARED_EXTENSION ""
#
#	define hyper_open_library$(library) ((void) 0)
#	define hyper_close_library$(library) ((void) 0)
#	define hyper_get_proc_address$(library, proc_name) ((void) 0)
#
#	define HYPER_API
#endif
