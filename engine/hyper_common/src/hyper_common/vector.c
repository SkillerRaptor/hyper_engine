/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/vector.h"

#include "hyper_common/assertion.h"

#include <stdlib.h>
#include <string.h>

static void hyper_vector_reallocate(
	struct hyper_vector *vector,
	size_t new_capacity)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(new_capacity != 0);

	if (new_capacity < vector->size)
	{
		vector->size = new_capacity;
	}

	void *new_data = malloc(new_capacity * vector->element_size);
	memcpy(new_data, vector->data, vector->size * vector->element_size);
	free(vector->data);

	vector->data = new_data;
	vector->capacity = new_capacity;
}

enum hyper_result hyper_vector_create(
	struct hyper_vector *vector,
	size_t element_size)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(element_size != 0);

	vector->size = 0;
	vector->capacity = 2;
	vector->element_size = element_size;
	vector->data = malloc(vector->capacity * vector->element_size);
	if (vector->data == NULL)
	{
		return HYPER_RESULT_OUT_OF_MEMORY;
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_vector_destroy(struct hyper_vector *vector)
{
	if (vector == NULL)
	{
		return;
	}

	free(vector->data);
}

void *hyper_vector_get(struct hyper_vector *vector, size_t index)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(index < vector->size);

	return ((uint8_t *) vector->data) + (index * vector->element_size);
}

void *hyper_vector_front(struct hyper_vector *vector)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(vector->size != 0);

	return hyper_vector_get(vector, 0);
}

void *hyper_vector_back(struct hyper_vector *vector)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(vector->size != 0);

	return hyper_vector_get(vector, vector->size - 1);
}

bool hyper_vector_empty(struct hyper_vector *vector)
{
	HYPER_ASSERT(vector != NULL);

	return vector->size == 0;
}

void hyper_vector_clear(struct hyper_vector *vector)
{
	HYPER_ASSERT(vector != NULL);

	hyper_vector_resize(vector, 0);
}

void hyper_vector_push_back(struct hyper_vector *vector, const void *element)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(element != NULL);

	if (vector->size >= vector->capacity)
	{
		hyper_vector_reallocate(vector, vector->capacity + vector->capacity / 2);
	}

	++vector->size;

	void *ptr = hyper_vector_get(vector, vector->size - 1);
	memcpy(ptr, element, vector->element_size);
}

void hyper_vector_pop_back(struct hyper_vector *vector)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(vector->size > 0);

	--vector->size;
}

void hyper_vector_resize(struct hyper_vector *vector, size_t size)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(size != 0);

	hyper_vector_reallocate(vector, size + size / 2);
	vector->size = size;
}

void hyper_vector_reserve(struct hyper_vector *vector, size_t capacity)
{
	HYPER_ASSERT(vector != NULL);
	HYPER_ASSERT(capacity != 0);

	hyper_vector_reallocate(vector, capacity);
}
