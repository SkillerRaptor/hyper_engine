/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/buffer.hpp"

namespace hyper_engine
{
    Buffer::Buffer(const BufferDescriptor &descriptor, ResourceHandle handle)
        : m_label(descriptor.label)
        , m_byte_size(descriptor.byte_size)
        , m_usage(descriptor.usage)
        , m_handle(handle)
    {
    }

    std::string_view Buffer::label() const
    {
        return m_label;
    }

    uint64_t Buffer::byte_size() const
    {
        return m_byte_size;
    }

    BitFlags<BufferUsage> Buffer::usage() const
    {
        return m_usage;
    }

    ResourceHandle Buffer::handle() const
    {
        return m_handle;
    }
} // namespace hyper_engine