/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/vector.h"

#include "hyper_common/assertion.h"
#include "hyper_common/memory.h"

#include <string.h>

static void vector_reallocate(struct hyper_vector *vector, size_t new_capacity)
{
	hyper_assert$(vector != NULL);
	hyper_assert$(new_capacity != 0);

	if (new_capacity < vector->size)
	{
		vector->size = new_capacity;
	}

	void *new_data = hyper_allocate(new_capacity * vector->element_size);
	memcpy(new_data, vector->data, vector->size * vector->element_size);
	hyper_deallocate(vector->data);

	vector->data = new_data;
	vector->capacity = new_capacity;
}

static void *vector_get_offset(struct hyper_vector *vector, size_t index)
{
	hyper_assert$(vector != NULL);

	return (uint8_t *) vector->data + (index * vector->element_size);
}

enum hyper_result hyper_vector_create(struct hyper_vector *vector, size_t element_size)
{
	hyper_assert$(vector != NULL);
	hyper_assert$(element_size != 0);

	vector->size = 0;
	vector->capacity = 2;
	vector->element_size = element_size;
	vector->data = hyper_allocate(vector->capacity * vector->element_size);

	if (vector->data == NULL)
	{
		return HYPER_RESULT_OUT_OF_MEMORY;
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_vector_destroy(struct hyper_vector *vector)
{
	hyper_assert$(vector != NULL);

	hyper_deallocate(vector->data);
	vector->data = NULL;
}

void hyper_vector_push_back(struct hyper_vector *vector, void *element)
{
	hyper_assert$(vector != NULL);
	hyper_assert$(element != NULL);

	if (vector->size >= vector->capacity)
	{
		vector_reallocate(vector, vector->capacity + vector->capacity / 2);
	}

	void *ptr = vector_get_offset(vector, vector->size);
	memcpy(ptr, element, vector->element_size);

	++vector->size;
}

void *hyper_vector_get(struct hyper_vector *vector, size_t index)
{
	hyper_assert$(vector != NULL);

	return vector_get_offset(vector, index);
}
