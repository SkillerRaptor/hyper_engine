/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include "hyper_rhi/surface.hpp"

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

        virtual std::unique_ptr<Surface> create_surface(const SurfaceDescriptor &descriptor) = 0;
    };
} // namespace hyper_rhi
