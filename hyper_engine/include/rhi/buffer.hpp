/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

#include <hyper_core/bit_flags.hpp>

#include "hyper_rhi/resource_handle.hpp"

namespace hyper_engine
{
    enum class BufferUsage : uint8_t
    {
        None = 0,
        Index = 1 << 0,
        Indirect = 1 << 1,
        Storage = 1 << 2,
        ShaderResource = 1 << 3,
    };

    struct BufferDescriptor
    {
        std::string label;
        uint64_t byte_size = 0;
        BitFlags<BufferUsage> usage = BufferUsage::None;
    };

    class Buffer
    {
    public:
        virtual ~Buffer() = default;

        std::string_view label() const;
        uint64_t byte_size() const;
        BitFlags<BufferUsage> usage() const;
        ResourceHandle handle() const;

    protected:
        Buffer(const BufferDescriptor &descriptor, ResourceHandle handle);

    protected:
        std::string m_label;
        uint64_t m_byte_size = 0;
        BitFlags<BufferUsage> m_usage = BufferUsage::None;
        ResourceHandle m_handle;
    };
} // namespace hyper_engine
