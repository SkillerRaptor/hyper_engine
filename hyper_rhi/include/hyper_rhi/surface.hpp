/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <hyper_platform/window.hpp>

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
    };

    using SurfaceHandle = std::shared_ptr<Surface>;
} // namespace hyper_rhi
