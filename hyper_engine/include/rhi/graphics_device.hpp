/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/memory.hpp"
#include "rhi/buffer.hpp"
#include "rhi/compute_pipeline.hpp"
#include "rhi/pipeline_layout.hpp"
#include "rhi/render_pipeline.hpp"
#include "rhi/sampler.hpp"
#include "rhi/shader.hpp"
#include "rhi/texture.hpp"

namespace he
{
    class Window;

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
    public:
        static RefPtr<GraphicsDevice> create(GraphicsApi, const Window &, Validation validation_requested);
        virtual ~GraphicsDevice() = default;

        GraphicsDevice(const GraphicsDevice &) = delete;
        GraphicsDevice &operator=(const GraphicsDevice &) = delete;

        GraphicsDevice(GraphicsDevice &&) noexcept = default;
        GraphicsDevice &operator=(GraphicsDevice &&) noexcept = default;

        virtual RefPtr<Buffer> create_buffer(const BufferDescriptor &) = 0;
        virtual RefPtr<Shader> create_shader(const ShaderDescriptor &) = 0;
        virtual RefPtr<Sampler> create_sampler(const SamplerDescriptor &) = 0;
        virtual RefPtr<Texture> create_texture(const TextureDescriptor &) = 0;
        virtual RefPtr<PipelineLayout> create_pipeline_layout(const PipelineLayoutDescriptor &) = 0;
        virtual RefPtr<ComputePipeline> create_compute_pipeline(const ComputePipelineDescriptor &) = 0;
        virtual RefPtr<RenderPipeline> create_render_pipeline(const RenderPipelineDescriptor &) = 0;

        // virtual void wait_idle() const = 0;

    protected:
        GraphicsDevice() = default;
    };
} // namespace he
