/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/ref_ptr.hpp>
#include <hyper_rhi/forward.hpp>

namespace hyper_engine
{
    struct DrawContext;

    class OpaquePass
    {
    public:
        OpaquePass(
            const RefPtr<TextureView> &render_texture_view,
            const RefPtr<TextureView> &depth_texture_view,
            const RefPtr<Buffer> &scene_buffer);

        void render(const RefPtr<CommandList> &command_list, const DrawContext &draw_context) const;

    private:
        const RefPtr<TextureView> &m_render_texture_view;
        const RefPtr<TextureView> &m_depth_texture_view;
        const RefPtr<Buffer> &m_scene_buffer;
    };
} // namespace hyper_engine