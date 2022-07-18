/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/queue.h"

#include "hyper_common/assertion.h"
#include "hyper_common/memory.h"

#include <string.h>

static void hyper_queue_reallocate(
	struct hyper_queue *queue,
	size_t new_capacity)
{
	hyper_assert$(queue != NULL);
	hyper_assert$(new_capacity != 0);

	if (new_capacity < queue->size)
	{
		queue->size = new_capacity;
	}

	void *new_data = hyper_allocate(new_capacity * queue->element_size);
	memcpy(new_data, queue->data, queue->size * queue->element_size);
	hyper_deallocate(queue->data);

	queue->data = new_data;
	queue->capacity = new_capacity;
}

static void *hyper_queue_get_offset(struct hyper_queue *queue, size_t index)
{
	hyper_assert$(queue != NULL);

	return (uint8_t *) queue->data + (index * queue->element_size);
}

enum hyper_result hyper_queue_create(
	struct hyper_queue *queue,
	size_t element_size)
{
	hyper_assert$(queue != NULL);
	hyper_assert$(element_size != 0);

	queue->size = 0;
	queue->capacity = 2;
	queue->element_size = element_size;
	queue->data = hyper_allocate(queue->capacity * queue->element_size);

	if (queue->data == NULL)
	{
		return HYPER_RESULT_OUT_OF_MEMORY;
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_queue_destroy(struct hyper_queue *queue)
{
	hyper_assert$(queue != NULL);

	hyper_deallocate(queue->data);
	queue->data = NULL;
}

void hyper_queue_push(struct hyper_queue *queue, void *element)
{
	hyper_assert$(queue != NULL);
	hyper_assert$(element != NULL);

	if (queue->size >= queue->capacity)
	{
		hyper_queue_reallocate(queue, queue->capacity + queue->capacity / 2);
	}

	void *ptr = hyper_queue_get_offset(queue, queue->size);
	memcpy(ptr, element, queue->element_size);

	++queue->size;
}

void hyper_queue_pop(struct hyper_queue *queue)
{
	hyper_assert$(queue != NULL);

	void *ptr = hyper_queue_get_offset(queue, 0);
	memmove(
		ptr,
		(uint8_t *) ptr + queue->element_size,
		(queue->size - 1) * queue->element_size);

	--queue->size;
}

void *hyper_queue_front(struct hyper_queue *queue)
{
	hyper_assert$(queue != NULL);

	return hyper_queue_get_offset(queue, 0);
}

void *hyper_queue_back(struct hyper_queue *queue)
{
	hyper_assert$(queue != NULL);

	return hyper_queue_get_offset(queue, queue->size - 1);
}
