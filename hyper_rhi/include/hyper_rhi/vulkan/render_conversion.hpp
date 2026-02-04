/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/buffer.hpp"

namespace he
{
    VkBufferUsageFlags map_buffer_usage(BitFlags<BufferUsage>);
} // namespace he
