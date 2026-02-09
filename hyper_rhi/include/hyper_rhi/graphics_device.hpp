/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/memory.hpp>
#include <hyper_core/types.hpp>
#include <hyper_platform/forward.hpp>

#include "hyper_rhi/forward.hpp"

namespace he
{
    enum class GraphicsApi
    {
        DirectX12,
        Vulkan,
    };

    enum class Validation
    {
        Enabled,
        Disabled,
    };

    class GraphicsDevice
    {
    protected:
        static constexpr usize s_frames_in_flight = 3;

    public:
        static OwnPtr<GraphicsDevice> create(GraphicsApi, const Window &, Validation validation_requested);
        virtual ~GraphicsDevice() = default;

        GraphicsDevice(const GraphicsDevice &) = delete;
        GraphicsDevice &operator=(const GraphicsDevice &) = delete;

        GraphicsDevice(GraphicsDevice &&) noexcept = default;
        GraphicsDevice &operator=(GraphicsDevice &&) noexcept = default;

        virtual RefPtr<Buffer> create_buffer(const BufferDescriptor &) = 0;
        virtual RefPtr<Shader> create_shader(const ShaderDescriptor &) = 0;
        virtual RefPtr<Sampler> create_sampler(const SamplerDescriptor &) = 0;
        virtual RefPtr<Texture> create_texture(const TextureDescriptor &) = 0;
        virtual RefPtr<TextureView> create_texture_view(const TextureViewDescriptor &) = 0;
        virtual RefPtr<PipelineLayout> create_pipeline_layout(const PipelineLayoutDescriptor &) = 0;
        virtual RefPtr<ComputePipeline> create_compute_pipeline(const ComputePipelineDescriptor &) = 0;
        virtual RefPtr<RenderPipeline> create_render_pipeline(const RenderPipelineDescriptor &) = 0;

        virtual void wait_idle() const = 0;

        CommandEncoder &acquire_command_encoder();
        void submit_command_encoder(CommandEncoder &);

    protected:
        GraphicsDevice() = default;

        virtual CommandEncoder &acquire_command_encoder_impl(u32 frame_id) = 0;
        virtual void submit_command_encoder_impl(CommandEncoder &) = 0;

    protected:
        u32 m_frame_index = 0;
    };
} // namespace he
