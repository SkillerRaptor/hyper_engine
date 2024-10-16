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
        std::shared_ptr<hyper_rhi::GraphicsDevice> graphics_device;
        std::shared_ptr<hyper_rhi::Surface> surface;
    };

    class Renderer
    {
    public:
        explicit Renderer(const RendererDescriptor &descriptor);

    private:
        std::shared_ptr<hyper_rhi::GraphicsDevice> m_graphics_device;
        std::shared_ptr<hyper_rhi::Surface> m_surface;
    };
} // namespace hyper_render