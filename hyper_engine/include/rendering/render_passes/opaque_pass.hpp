/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "rhi/buffer.hpp"
#include "rhi/command_list.hpp"
#include "rhi/texture_view.hpp"

#include <memory>

namespace hyper_engine
{
    class OpaquePass
    {
    public:
        OpaquePass(
            const std::shared_ptr<TextureView> &render_texture_view,
            const std::shared_ptr<TextureView> &depth_texture_view,
            const std::shared_ptr<Buffer> &scene_buffer);

        void render(const std::shared_ptr<CommandList> &command_list, const DrawContext &draw_context) const;

    private:
        const std::shared_ptr<TextureView> &m_render_texture_view;
        const std::shared_ptr<TextureView> &m_depth_texture_view;
        const std::shared_ptr<Buffer> &m_scene_buffer;
    };
} // namespace hyper_engine