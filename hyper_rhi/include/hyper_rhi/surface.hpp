/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <hyper_platform/window.hpp>

#include "hyper_rhi/texture.hpp"

namespace hyper_rhi
{
    struct SurfaceDescriptor
    {
        hyper_platform::Window &window;
    };

    class Surface
    {
    public:
        virtual ~Surface() = default;

        virtual void resize(uint32_t width, uint32_t height) = 0;

        [[nodiscard]] virtual TextureHandle current_texture() const = 0;
    };

    using SurfaceHandle = std::shared_ptr<Surface>;
} // namespace hyper_rhi
