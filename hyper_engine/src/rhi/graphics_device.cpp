/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/assertion.hpp>

#if HE_WINDOWS
// #    include "hyper_rhi/d3d12/d3d12_graphics_device.hpp"
#endif

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/descriptor_manager.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/render_pipeline.hpp"
#include "hyper_rhi/shader_module.hpp"
#include "hyper_rhi/sampler.hpp"
#include "hyper_rhi/texture.hpp"
#include "hyper_rhi/texture_view.hpp"
#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

namespace hyper_engine
{
    std::unique_ptr<GraphicsDevice> GraphicsDevice::create(const Logger &logger, const GraphicsDeviceDescriptor &descriptor)
    {
        switch (descriptor.graphics_api)
        {
        case GraphicsApi::D3D12:
#if HE_WINDOWS
            HE_PANIC();
#else
            return nullptr;
#endif
        case GraphicsApi::Vulkan:
            return std::make_unique<VulkanGraphicsDevice>(logger, descriptor);
        default:
            HE_UNREACHABLE();
        }
    }

    std::shared_ptr<Buffer> GraphicsDevice::create_buffer(const BufferDescriptor &descriptor)
    {
        ResourceHandle handle;
        if (descriptor.usage & BufferUsage::ShaderResource)
        {
            handle = descriptor_manager().allocate_handle();
        }

        return create_buffer(descriptor, handle);
    }

    std::shared_ptr<Buffer> GraphicsDevice::create_buffer(const BufferDescriptor &descriptor, const ResourceHandle handle)
    {
        HE_ASSERT(descriptor.byte_size > 0);
        HE_ASSERT(descriptor.usage != BufferUsage::None);

        if (handle.is_valid())
        {
            HE_ASSERT(descriptor.usage & BufferUsage::ShaderResource);
        }

        if (descriptor.usage & BufferUsage::ShaderResource)
        {
            HE_ASSERT(descriptor.usage & BufferUsage::Storage);
        }

        const std::shared_ptr<Buffer> buffer = create_buffer_platform(descriptor, handle);

        // FIXME: Could this be written cleaner?
        if (descriptor.usage & BufferUsage::ShaderResource)
        {
            descriptor_manager().set_buffer(buffer, handle);
        }

        return buffer;
    }

    std::shared_ptr<ComputePipeline> GraphicsDevice::create_compute_pipeline(const ComputePipelineDescriptor &descriptor)
    {
        HE_ASSERT(descriptor.layout);
        HE_ASSERT(descriptor.shader);

        return create_compute_pipeline_platform(descriptor);
    }

    std::shared_ptr<RenderPipeline> GraphicsDevice::create_render_pipeline(const RenderPipelineDescriptor &descriptor)
    {
        HE_ASSERT(descriptor.layout);
        HE_ASSERT(descriptor.vertex_shader);
        HE_ASSERT(descriptor.fragment_shader);
        HE_ASSERT(!descriptor.color_attachment_states.empty());

        for (const ColorAttachmentState &color_attachment_state : descriptor.color_attachment_states)
        {
            HE_ASSERT(color_attachment_state.format != Format::Unknown);
        }

        if (descriptor.depth_stencil_state.depth_test_enable)
        {
            HE_ASSERT(descriptor.depth_stencil_state.depth_format != Format::Unknown);
        }

        return create_render_pipeline_platform(descriptor);
    }

    std::shared_ptr<PipelineLayout> GraphicsDevice::create_pipeline_layout(const PipelineLayoutDescriptor &descriptor)
    {
        HE_ASSERT((descriptor.push_constant_size % 4) == 0);

        return create_pipeline_layout_platform(descriptor);
    }

    std::shared_ptr<ShaderModule> GraphicsDevice::create_shader_module(const ShaderModuleDescriptor &descriptor)
    {
        HE_ASSERT(descriptor.type != ShaderType::None);
        HE_ASSERT(!descriptor.entry_name.empty());
        HE_ASSERT(!descriptor.bytes.empty());

        return create_shader_module_platform(descriptor);
    }

    std::shared_ptr<Sampler> GraphicsDevice::create_sampler(const SamplerDescriptor &descriptor)
    {
        const ResourceHandle handle = descriptor_manager().allocate_handle();

        return create_sampler(descriptor, handle);
    }

    std::shared_ptr<Sampler> GraphicsDevice::create_sampler(const SamplerDescriptor &descriptor, const ResourceHandle handle)
    {
        // FIXME: Add assertions

        const std::shared_ptr<Sampler> sampler = create_sampler_platform(descriptor, handle);

        // FIXME: Could this be written cleaner?
        descriptor_manager().set_sampler(sampler, handle);

        return sampler;
    }

    std::shared_ptr<Texture> GraphicsDevice::create_texture(const TextureDescriptor &descriptor)
    {
        HE_ASSERT(descriptor.width > 0);
        HE_ASSERT(descriptor.height > 0);
        HE_ASSERT(descriptor.depth > 0);
        HE_ASSERT(descriptor.array_size > 0);
        HE_ASSERT(descriptor.mip_levels > 0);
        HE_ASSERT(descriptor.format != Format::Unknown);
        HE_ASSERT(descriptor.dimension != Dimension::Unknown);
        HE_ASSERT(descriptor.usage != TextureUsage::None);

        // FIXME: Add check that sampled and storage image can't be used simultaneously (exclusive)

        return create_texture_platform(descriptor);
    }

    std::shared_ptr<TextureView> GraphicsDevice::create_texture_view(const TextureViewDescriptor &descriptor)
    {
        ResourceHandle handle;
        if (descriptor.texture->usage() & TextureUsage::ShaderResource)
        {
            handle = descriptor_manager().allocate_handle();
        }

        return create_texture_view(descriptor, handle);
    }

    std::shared_ptr<TextureView> GraphicsDevice::create_texture_view(const TextureViewDescriptor &descriptor, const ResourceHandle handle)
    {
        HE_ASSERT(descriptor.texture);
        HE_ASSERT(descriptor.subresource_range.mip_level_count > 0);
        HE_ASSERT(descriptor.subresource_range.array_layer_count > 0);

        const std::shared_ptr<TextureView> texture_view = create_texture_view_platform(descriptor, handle);

        // FIXME: Could this be written cleaner?
        if (descriptor.texture->usage() & TextureUsage::ShaderResource)
        {
            if (descriptor.texture->usage() & TextureUsage::Storage)
            {
                descriptor_manager().set_storage_image(texture_view, handle);
            }
            else
            {
                descriptor_manager().set_sampled_image(texture_view, handle);
            }
        }

        return texture_view;
    }
} // namespace hyper_engine
