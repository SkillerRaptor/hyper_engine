/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

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
} // namespace hyper_rhi
