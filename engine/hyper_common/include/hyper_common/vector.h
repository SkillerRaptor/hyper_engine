/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"

#include <stddef.h>

struct hyper_vector
{
	size_t size;
	size_t capacity;

	size_t element_size;
	void *data;
};

enum hyper_result hyper_vector_create(
	struct hyper_vector *vector,
	size_t element_size);
void hyper_vector_destroy(struct hyper_vector *vector);

void hyper_vector_push_back(struct hyper_vector *vector, void *element);

void *hyper_vector_get(struct hyper_vector *vector, size_t index);
