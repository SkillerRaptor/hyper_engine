/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/assertion.hpp>

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/render_pipeline.hpp"
#include "hyper_rhi/sampler.hpp"
#include "hyper_rhi/shader.hpp"
#include "hyper_rhi/texture.hpp"
#include "hyper_rhi/texture_view.hpp"
#include "hyper_rhi/validation.hpp"
#include "hyper_rhi/vulkan/graphics_device.hpp"

namespace he
{
    OwnPtr<GraphicsDevice> GraphicsDevice::create(
        const GraphicsApi graphics_api, const Window &window, const Validation validation_requested)
    {
        switch (graphics_api)
        {
        case GraphicsApi::Vulkan:
            return make_own<VulkanGraphicsDevice>(window, validation_requested);
        default:
            HE_UNREACHABLE();
        }
    }

    Buffer GraphicsDevice::create_buffer(const BufferDescriptor &desc)
    {
        validate_buffer_descriptor(desc);
        return create_buffer_impl(desc);
    }

    Shader GraphicsDevice::create_shader(const ShaderDescriptor &desc)
    {
        validate_shader_descriptor(desc);
        return create_shader_impl(desc);
    }

    Sampler GraphicsDevice::create_sampler(const SamplerDescriptor &desc)
    {
        validate_sampler_descriptor(desc);
        return create_sampler_impl(desc);
    }

    Texture GraphicsDevice::create_texture(const TextureDescriptor &desc)
    {
        validate_texture_descriptor(desc);
        return create_texture_impl(desc);
    }

    TextureView GraphicsDevice::create_texture_view(const TextureViewDescriptor &desc)
    {
        validate_texture_view_descriptor(desc);
        return create_texture_view_impl(desc);
    }

    PipelineLayout GraphicsDevice::create_pipeline_layout(const PipelineLayoutDescriptor &desc)
    {
        validate_pipeline_layout_descriptor(desc);
        return create_pipeline_layout_impl(desc);
    }

    ComputePipeline GraphicsDevice::create_compute_pipeline(const ComputePipelineDescriptor &desc)
    {
        validate_compute_pipeline_descriptor(desc);
        return create_compute_pipeline_impl(desc);
    }

    RenderPipeline GraphicsDevice::create_render_pipeline(const RenderPipelineDescriptor &desc)
    {
        validate_render_pipeline_descriptor(desc);
        return create_render_pipeline_impl(desc);
    }

    CommandEncoder GraphicsDevice::acquire_command_encoder()
    {
        const u32 frame_id = m_frame_index % s_frames_in_flight;
        return acquire_command_encoder_impl(frame_id);
    }

    void GraphicsDevice::submit_command_encoder(CommandEncoder command_encoder)
    {
        submit_command_encoder_impl(std::move(command_encoder));
        m_frame_index += 1;
    }
} // namespace he
