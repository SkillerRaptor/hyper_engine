/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/module_loader.h"

#include "hyper_common/assertion.h"
#include "hyper_common/library.h"
#include "hyper_common/logger.h"
#include "hyper_common/memory.h"

#include <string.h>

enum hyper_result hyper_module_loader_create(
	struct hyper_module_loader *module_loader)
{
	hyper_assert$(module_loader != NULL);

	if (
		hyper_vector_create(&module_loader->modules, sizeof(struct hyper_module)) !=
		HYPER_RESULT_SUCCESS)
	{
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_module_loader_destroy(struct hyper_module_loader *module_loader)
{
	hyper_assert$(module_loader != NULL);
	
	for (size_t i = 0; i < module_loader->modules.size; ++i)
	{
		struct hyper_module *module = hyper_vector_get(&module_loader->modules, i);
		hyper_module_loader_unload(module_loader, module->name);
	}

	hyper_vector_destroy(&module_loader->modules);
}

enum hyper_result hyper_module_loader_load(
	struct hyper_module_loader *module_loader,
	const char *module_name)
{
	// TODO: Replace vector with map
	for (size_t i = 0; i < module_loader->modules.size; ++i)
	{
		struct hyper_module *module = hyper_vector_get(&module_loader->modules, i);
		if (strncmp(module_name, module->name, strlen(module_name)) == 0)
		{
			hyper_logger_error$(
				"Failed to add module with same name '%s'\n", module->name);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	const uint8_t prefix_length = strlen(HYPER_SHARED_PREFIX);
	const uint16_t module_name_length = strlen(module_name);
	const uint8_t extension_length = strlen(HYPER_SHARED_EXTENSION);
	const size_t library_name_length =
		prefix_length + module_name_length + extension_length;
	char *library_name = hyper_callocate((library_name_length + 1) * sizeof(char));

	strcat(library_name, HYPER_SHARED_PREFIX);
	strcat(library_name, module_name);
	strcat(library_name, HYPER_SHARED_EXTENSION);

	void *library_handle = hyper_open_library$(library_name);
	if (library_handle == NULL)
	{
		hyper_logger_error$("Failed to open library file '%s'\n", library_name);
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_deallocate(library_name);

	struct hyper_module module = {
		.name = hyper_callocate((strlen(module_name) + 1) * sizeof(char)),
		.library_handle = library_handle,
	};

	strcpy(module.name, module_name);

	hyper_vector_push_back(&module_loader->modules, &module);

	hyper_logger_info$("Loaded module '%s'\n", module_name);

	return HYPER_RESULT_SUCCESS;
}

void hyper_module_loader_unload(
	struct hyper_module_loader *module_loader,
	const char *module_name)
{
	// TODO: Replace vector with map
	for (size_t i = 0; i < module_loader->modules.size; ++i)
	{
		struct hyper_module *module = hyper_vector_get(&module_loader->modules, i);
		if (strncmp(module_name, module->name, strlen(module_name)) != 0)
		{
			continue;
		}

		hyper_logger_info$("Unloaded module '%s'\n", module_name);

		hyper_close_library$(module->library_handle);
		hyper_deallocate(module->name);
		// TODO: Erase from vector/map

		return;
	}
}
