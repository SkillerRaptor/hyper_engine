/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

namespace hyper_rhi
{
    enum class GraphicsApi
    {
        D3D12,
        Vulkan,
    };

    struct GraphicsDeviceDescriptor
    {
        GraphicsApi graphics_api;
        bool debug_mode;
    };

    class GraphicsDevice
    {
    public:
        static std::unique_ptr<GraphicsDevice> create(const GraphicsDeviceDescriptor &descriptor);
        virtual ~GraphicsDevice() = default;
    };
} // namespace hyper_rhi
