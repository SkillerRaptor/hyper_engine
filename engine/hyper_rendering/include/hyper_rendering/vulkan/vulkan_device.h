/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"
#include "hyper_rendering/vulkan/vulkan_context.h"

#include <stdbool.h>
#include <stdint.h>

struct hyper_queue_families
{
	uint32_t graphics_family;
	bool graphics_family_valid;
};

enum hyper_result hyper_vulkan_device_create(
	struct hyper_vulkan_context *vulkan_context);
void hyper_vulkan_device_destroy(struct hyper_vulkan_context *vulkan_context);

struct hyper_queue_families hyper_find_queue_families(
	VkPhysicalDevice physical_device);
