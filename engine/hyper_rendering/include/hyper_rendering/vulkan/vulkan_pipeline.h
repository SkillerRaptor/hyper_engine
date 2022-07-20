/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"
#include "hyper_rendering/vulkan/vulkan_context.h"

enum hyper_result hyper_vulkan_pipeline_create(
	struct hyper_vulkan_context *vulkan_context);
void hyper_vulkan_pipeline_destroy(struct hyper_vulkan_context *vulkan_context);
