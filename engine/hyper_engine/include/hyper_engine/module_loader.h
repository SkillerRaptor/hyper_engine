/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/memory.h"
#include "hyper_common/result.h"
#include "hyper_common/vector.h"

struct hyper_module
{
	char *name;
	void *library_handle;
};

struct hyper_module_loader
{
	void *allocation_debug_info;
	struct hyper_vector modules;
};

enum hyper_result hyper_module_loader_create(
	struct hyper_module_loader *module_loader);
void hyper_module_loader_destroy(struct hyper_module_loader *module_loader);

enum hyper_result hyper_module_loader_load(
	struct hyper_module_loader *module_loader,
	const char *module_name);
void hyper_module_loader_unload(
	struct hyper_module_loader *module_loader,
	const char *module_name);
