/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_render/renderer.hpp"

namespace hyper_render
{
    Renderer::Renderer(const RendererDescriptor &descriptor)
        : m_graphics_device(descriptor.graphics_device)
        , m_surface(descriptor.surface)
    {
    }
} // namespace hyper_render