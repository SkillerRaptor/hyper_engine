/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_rhi/graphics_device.hpp>
#include <hyper_rhi/surface.hpp>

namespace hyper_render
{
    struct RendererDescriptor
    {
        hyper_rhi::GraphicsDeviceHandle graphics_device;
        hyper_rhi::SurfaceHandle surface;
    };

    class Renderer
    {
    public:
        explicit Renderer(const RendererDescriptor &descriptor);

    private:
        hyper_rhi::GraphicsDeviceHandle m_graphics_device;
        hyper_rhi::SurfaceHandle m_surface;
    };
} // namespace hyper_render