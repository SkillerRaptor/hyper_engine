/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"

#include <stddef.h>

struct hyper_queue
{
	size_t size;
	size_t capacity;

	size_t element_size;
	void *data;
};

enum hyper_result hyper_queue_create(
	struct hyper_queue *queue,
	size_t element_size);
void hyper_queue_destroy(struct hyper_queue *queue);

void hyper_queue_push(struct hyper_queue *queue, void *element);
void hyper_queue_pop(struct hyper_queue *queue);

void *hyper_queue_front(struct hyper_queue *queue);
void *hyper_queue_back(struct hyper_queue *queue);
