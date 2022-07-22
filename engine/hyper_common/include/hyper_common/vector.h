/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"

#include <stddef.h>

#define hyper_vector_foreach$(vector, type, name)                            \
	for (size_t vector_index = 0; vector_index < (vector).size;            \
			 ++vector_index)                                                       \
		for (type *name = hyper_vector_get(&vector, vector_index); name != NULL; \
				 name = NULL)

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

void hyper_vector_push_back(struct hyper_vector *vector, const void *element);

void hyper_vector_resize(struct hyper_vector *vector, size_t size);
void hyper_vector_reserve(struct hyper_vector *vector, size_t capacity);

void *hyper_vector_get(struct hyper_vector *vector, size_t index);
