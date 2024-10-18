/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>

namespace hyper_rhi
{
    struct BufferDescriptor
    {
        std::string label;

        uint64_t byte_size = 0;
        bool is_index_buffer = false;
        bool is_constant_buffer = false;
    };

    class Buffer
    {
    public:
        virtual ~Buffer() = default;
    };

    using BufferHandle = std::shared_ptr<Buffer>;
} // namespace hyper_rhi
