/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/queue.h"

#include "hyper_common/assertion.h"

#include <stdlib.h>
#include <string.h>

static void hyper_queue_reallocate(struct hyper_queue *queue, size_t new_size)
{
	HYPER_ASSERT(queue != NULL);
	HYPER_ASSERT(new_size != 0);

	void *new_data = malloc(new_size * queue->element_size);
	memcpy(new_data, queue->data, queue->size * queue->element_size);
	free(queue->data);

	queue->data = new_data;
	queue->size = new_size;
}

static void *hyper_queue_get(struct hyper_queue *queue, size_t index)
{
	HYPER_ASSERT(queue != NULL);
	HYPER_ASSERT(index < queue->size);

	return ((uint8_t *) queue->data) + (index * queue->element_size);
}

enum hyper_result hyper_queue_create(
	struct hyper_queue *queue,
	size_t element_size)
{
	HYPER_ASSERT(queue != NULL);
	HYPER_ASSERT(element_size != 0);

	queue->size = 0;
	queue->element_size = element_size;
	queue->data = NULL;

	return HYPER_RESULT_SUCCESS;
}

void hyper_queue_destroy(struct hyper_queue *queue)
{
	if (queue == NULL)
	{
		return;
	}

	free(queue->data);
}

void *hyper_queue_front(struct hyper_queue *queue)
{
	HYPER_ASSERT(queue != NULL);
	HYPER_ASSERT(queue->size != 0);

	return hyper_queue_get(queue, 0);
}

void *hyper_queue_back(struct hyper_queue *queue)
{
	HYPER_ASSERT(queue != NULL);
	HYPER_ASSERT(queue->size != 0);

	return hyper_queue_get(queue, queue->size - 1);
}

bool hyper_queue_empty(struct hyper_queue *queue)
{
	HYPER_ASSERT(queue != NULL);

	return queue->size == 0;
}

void hyper_queue_push(struct hyper_queue *queue, const void *element)
{
	HYPER_ASSERT(queue != NULL);
	HYPER_ASSERT(element != NULL);

	++queue->size;

	hyper_queue_reallocate(queue, queue->size);

	void *ptr = hyper_queue_get(queue, queue->size - 1);
	memcpy(ptr, element, queue->element_size);
}

void hyper_queue_pop(struct hyper_queue *queue)
{
	HYPER_ASSERT(queue != NULL);
	HYPER_ASSERT(queue->size > 0);

	--queue->size;

	void *ptr = hyper_queue_get(queue, 0);
	memmove(
		ptr,
		((uint8_t *) ptr) + (1 * queue->element_size),
		queue->size * queue->element_size);

	hyper_queue_reallocate(queue, queue->size - 1);
}
