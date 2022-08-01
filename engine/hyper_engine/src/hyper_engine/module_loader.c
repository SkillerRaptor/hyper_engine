/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/module_loader.h"

#include "hyper_common/assertion.h"
#include "hyper_common/library.h"
#include "hyper_common/logger.h"
#include "hyper_common/platform.h"
#include "hyper_common/strings.h"

#include <stdlib.h>
#include <string.h>

struct hyper_module
{
	char *name;
	struct hyper_library library;
};

enum hyper_result hyper_module_loader_create(
	struct hyper_module_loader *module_loader)
{
	HYPER_ASSERT(module_loader != NULL);

	if (
		hyper_vector_create(&module_loader->modules, sizeof(struct hyper_module)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to create 'modules' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_logger_info("Successfully created module loader\n");

	return HYPER_RESULT_SUCCESS;
}

void hyper_module_loader_destroy(struct hyper_module_loader *module_loader)
{
	if (module_loader == NULL)
	{
		return;
	}

	HYPER_VECTOR_FOREACH_REVERSE(
		module_loader->modules, i, struct hyper_module, module)
	{
		hyper_library_close(&module->library);

		hyper_logger_info("Successfully unloaded module '%s'\n", module->name);

		free(module->name);
	}

	hyper_vector_destroy(&module_loader->modules);

	hyper_logger_info("Successfully destroyed module loader\n");
}

enum hyper_result hyper_module_loader_load(
	struct hyper_module_loader *module_loader,
	const char *module_name)
{
	// TODO: The approach of using a vector is temporary and should be replaced
	// with a (hash) map to provide faster iterations and faster access
	HYPER_VECTOR_FOREACH (module_loader->modules, i, struct hyper_module, module)
	{
		const size_t module_name_length = strlen(module->name);
		if (strncmp(module_name, module->name, module_name_length) == 0)
		{
			hyper_logger_error(
				"Failed to load already existing module '%s'\n", module->name);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	char *library_name = hyper_strings_concatenate(
		3, HYPER_SHARED_PREFIX, module_name, HYPER_SHARED_EXTENSION);

	struct hyper_library library = { 0 };
	if (hyper_library_open(&library, library_name) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to open library file '%s'\n", library_name);
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	free(library_name);

	const size_t module_name_length = strlen(module_name);
	char *name = calloc(1, (module_name_length + 1) * sizeof(char));
	strcpy(name, module_name);

	struct hyper_module module = {
		.name = name,
		.library = library,
	};

	hyper_vector_push_back(&module_loader->modules, &module);

	hyper_logger_info("Successfully loaded module '%s'\n", module_name);

	return HYPER_RESULT_SUCCESS;
}
