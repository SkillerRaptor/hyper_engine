/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>

#include "hyper_rhi/texture.hpp"

namespace hyper_rhi
{
    struct RenderPassDescriptor
    {
        std::string label;

        std::shared_ptr<Texture> color_attachment = nullptr;
    };

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
    };
} // namespace hyper_rhi
