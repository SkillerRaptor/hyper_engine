/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "render/render_server.hpp"

#include "core/assertion.hpp"
#include "core/logger.hpp"
#include "render/render_driver.hpp"

std::unique_ptr<RenderServer> RenderServer::create(
    EventServer &event_server, void *native_window, const u32 width, const u32 height)
{
    HE_ASSERT(native_window != nullptr);

    std::unique_ptr<RenderDriver> render_driver = RenderDriver::create(GraphicsApi::Vulkan, native_window, width, height);
    HE_ASSERT(render_driver != nullptr);

    std::array<CommandBufferId, s_frames_in_flight> command_buffers {};
    for (uint8_t i = 0; i < s_frames_in_flight; ++i)
    {
        command_buffers[i] = render_driver->create_command_buffer();
    }

    const std::unordered_set<TextureViewId> texture_views
        = render_driver->query_swapchain_texture_views() | std::ranges::to<std::unordered_set<TextureViewId>>();

    HE_INFO("Initialized render server");
    return std::unique_ptr<RenderServer>(
        new RenderServer(event_server, std::move(render_driver), std::move(command_buffers), std::move(texture_views)));
}

RenderServer::RenderServer(EventServer &event_server,
    std::unique_ptr<RenderDriver> render_driver,
    std::array<CommandBufferId, s_frames_in_flight> command_buffers,
    std::unordered_set<TextureViewId> texture_views)
    : m_render_driver(std::move(render_driver))
    , m_command_buffers(std::move(command_buffers))
    , m_swapchain_texture_views(std::move(texture_views))
{
    event_server.subscribe<WindowResizeEvent>(HE_BIND_FUNCTION(on_resize));
}

RenderServer::~RenderServer()
{
    m_render_driver->wait_idle();

    for (const Resource &resource : m_deletion_queue)
    {
        switch (resource.tag)
        {
        case ResourceTag::Buffer: m_render_driver->destroy_buffer(resource.buffer_id); break;
        case ResourceTag::Shader: m_render_driver->destroy_shader(resource.shader_id); break;
        case ResourceTag::Sampler: m_render_driver->destroy_sampler(resource.sampler_id); break;
        case ResourceTag::Texture: m_render_driver->destroy_texture(resource.texture_id); break;
        case ResourceTag::TextureView: m_render_driver->destroy_texture_view(resource.texture_view_id); break;
        case ResourceTag::PipelineLayout: m_render_driver->destroy_pipeline_layout(resource.pipeline_layout_id); break;
        case ResourceTag::ComputePipeline: m_render_driver->destroy_compute_pipeline(resource.compute_pipeline_id); break;
        case ResourceTag::RenderPipeline: m_render_driver->destroy_render_pipeline(resource.render_pipeline_id); break;
        default: break;
        }
    }

    if (m_buffers.size() > 0)
    {
        HE_WARN("{} buffers still alive!", m_buffers.size());
        for (const BufferId buffer : m_buffers)
        {
            m_render_driver->destroy_buffer(buffer);
        }
    }

    if (m_shaders.size() > 0)
    {
        HE_WARN("{} shaders still alive!", m_shaders.size());
        for (const ShaderId shader : m_shaders)
        {
            m_render_driver->destroy_shader(shader);
        }
    }

    if (m_samplers.size() > 0)
    {
        HE_WARN("{} samplers still alive!", m_samplers.size());
        for (const SamplerId sampler : m_samplers)
        {
            m_render_driver->destroy_sampler(sampler);
        }
    }

    if (m_textures.size() > 0)
    {
        HE_WARN("{} textures still alive!", m_textures.size());
        for (const TextureId texture : m_textures)
        {
            m_render_driver->destroy_texture(texture);
        }
    }

    if (m_pipeline_layouts.size() > 0)
    {
        HE_WARN("{} pipeline layouts still alive!", m_pipeline_layouts.size());
        for (const PipelineLayoutId pipeline_layout : m_pipeline_layouts)
        {
            m_render_driver->destroy_pipeline_layout(pipeline_layout);
        }
    }

    if (m_compute_pipelines.size() > 0)
    {
        HE_WARN("{} compute pipelines still alive!", m_compute_pipelines.size());
        for (const ComputePipelineId compute_pipeline : m_compute_pipelines)
        {
            m_render_driver->destroy_compute_pipeline(compute_pipeline);
        }
    }

    if (m_render_pipelines.size() > 0)
    {
        HE_WARN("{} render pipelines still alive!", m_render_pipelines.size());
        for (const RenderPipelineId render_pipeline : m_render_pipelines)
        {
            m_render_driver->destroy_render_pipeline(render_pipeline);
        }
    }

    for (uint8_t i = 0; i < s_frames_in_flight; ++i)
    {
        m_render_driver->destroy_command_buffer(m_command_buffers[i]);
    }

    HE_INFO("Shutdown render server");
}

void RenderServer::wait_idle() const { m_render_driver->wait_idle(); }

BufferId RenderServer::create_buffer(const BufferDescriptor &desc)
{
    if (desc.label.has_value())
    {
        HE_ASSERT(!desc.label.value().empty());
    }

    HE_ASSERT(desc.size > 0);
    HE_ASSERT(desc.usage != BufferUsage::None);

    std::optional<ResourceHandle> handle = desc.handle;
    if (!handle.has_value() && ((desc.usage & BufferUsage::Resource) == BufferUsage::Resource))
    {
        handle = allocate_handle();
    }

    const BufferId id = m_render_driver->create_buffer(desc);
    Buffer *buffer = id.as<Buffer>();
    buffer->desc = desc;
    buffer->desc.handle = handle;
    m_buffers.insert(id);

    if (buffer->desc.handle.has_value())
    {
        m_render_driver->bind_buffer_to_slot(id, buffer->desc.handle.value());
    }

    return id;
}

void RenderServer::destroy_buffer(const BufferId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_buffers.contains(id));

    m_buffers.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::Buffer,
        .buffer_id = id,
        .generation = 0,
    });
}

ResourceHandle RenderServer::get_buffer_handle(const BufferId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_buffers.contains(id));

    const Buffer *buffer = id.as<Buffer>();
    HE_ASSERT((buffer->desc.usage & BufferUsage::Resource) == BufferUsage::Resource);
    HE_ASSERT(buffer->desc.handle.has_value());

    return buffer->desc.handle.value();
}

ShaderId RenderServer::create_shader(const ShaderDescriptor &desc)
{
    if (desc.label.has_value())
    {
        HE_ASSERT(!desc.label.value().empty());
    }

    HE_ASSERT(desc.type != ShaderType::None);
    HE_ASSERT(!desc.entry.empty());
    HE_ASSERT(!desc.path.empty());

    const ShaderId id = m_render_driver->create_shader(desc);
    Shader *shader = id.as<Shader>();
    shader->desc = desc;
    m_shaders.insert(id);
    return id;
}

void RenderServer::destroy_shader(const ShaderId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_shaders.contains(id));

    m_shaders.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::Shader,
        .shader_id = id,
        .generation = 0,
    });
}

SamplerId RenderServer::create_sampler(const SamplerDescriptor &desc)
{
    if (desc.label.has_value())
    {
        HE_ASSERT(!desc.label.value().empty());
    }

    HE_ASSERT(desc.min_lod >= 0.0f);
    HE_ASSERT(desc.max_lod >= desc.min_lod);

    std::optional<ResourceHandle> handle = desc.handle;
    if (!handle.has_value())
    {
        handle = allocate_handle();
    }

    const SamplerId id = m_render_driver->create_sampler(desc);
    Sampler *sampler = id.as<Sampler>();
    sampler->desc = desc;
    sampler->desc.handle = handle;
    m_samplers.insert(id);

    if (sampler->desc.handle.has_value())
    {
        m_render_driver->bind_sampler_to_slot(id, sampler->desc.handle.value());
    }

    return id;
}

void RenderServer::destroy_sampler(const SamplerId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_samplers.contains(id));

    m_samplers.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::Sampler,
        .sampler_id = id,
        .generation = 0,
    });
}

ResourceHandle RenderServer::get_sampler_handle(const SamplerId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_samplers.contains(id));

    const Sampler *sampler = id.as<Sampler>();
    HE_ASSERT(sampler->desc.handle.has_value());

    return sampler->desc.handle.value();
}

TextureId RenderServer::create_texture(const TextureDescriptor &desc)
{
    if (desc.label.has_value())
    {
        HE_ASSERT(!desc.label.value().empty());
    }

    HE_ASSERT(desc.extent.width > 0);
    HE_ASSERT(desc.extent.height > 0);
    HE_ASSERT(desc.extent.depth > 0);

    const uint32_t max_mip_levels
        = std::min(static_cast<uint32_t>(floor(log2(std::max(desc.extent.width, desc.extent.height))) + 1), 16u);
    HE_ASSERT(desc.mip_levels > 0);
    HE_ASSERT(desc.mip_levels <= max_mip_levels);

    HE_ASSERT(desc.format != Format::Unknown);
    HE_ASSERT(desc.dimension != Dimension::Unknown);

    if (desc.format == Format::D16Unorm || desc.format == Format::D32Sfloat || desc.format == Format::S8Uint
        || desc.format == Format::D16UnormS8Uint || desc.format == Format::D24UnormS8Uint
        || desc.format == Format::D32SfloatS8Uint)
    {
        HE_ASSERT(desc.dimension == Dimension::D2);
    }

    HE_ASSERT(desc.usage != TextureUsage::None);

    if ((desc.usage & TextureUsage::RenderAttachment) == TextureUsage::RenderAttachment)
    {
        HE_ASSERT(desc.dimension == Dimension::D2 || desc.dimension == Dimension::D3);
    }

    const TextureId id = m_render_driver->create_texture(desc);
    Texture *texture = id.as<Texture>();
    texture->desc = desc;
    m_textures.insert(id);
    return id;
}

void RenderServer::destroy_texture(const TextureId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_textures.contains(id));

    m_textures.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::Texture,
        .texture_id = id,
        .generation = 0,
    });
}

void RenderServer::generate_mip_maps(const CommandBufferId id, const TextureId texture_id)
{
    HE_ASSERT(texture_id.is_valid());
    HE_ASSERT(m_textures.contains(texture_id));

    m_render_driver->generate_mip_maps(id, texture_id);
}

TextureViewId RenderServer::create_texture_view(const TextureViewDescriptor &desc)
{
    if (desc.label.has_value())
    {
        const std::string &label = desc.label.value();
        HE_ASSERT(!label.empty());
    }

    HE_ASSERT(desc.texture.is_valid());
    HE_ASSERT(m_textures.contains(desc.texture));

    Texture *texture = desc.texture.as<Texture>();

    std::optional<ResourceHandle> handle = desc.handle;
    if (!handle.has_value() && ((texture->desc.usage & TextureUsage::Resource) == TextureUsage::Resource))
    {
        handle = allocate_handle();
    }

    // FIXME: Add more checks by comparing to the original texture

    const TextureViewId id = m_render_driver->create_texture_view(desc);
    TextureView *texture_view = id.as<TextureView>();
    texture_view->desc = desc;
    texture_view->desc.handle = handle;
    m_texture_views.insert(id);

    if (texture_view->desc.handle.has_value())
    {
        m_render_driver->bind_texture_view_to_slot(id, texture_view->desc.handle.value());
    }

    texture->views.push_back(id);

    return id;
}

void RenderServer::destroy_texture_view(const TextureViewId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_texture_views.contains(id));

    const TextureView *texture_view = id.as<TextureView>();
    Texture *texture = texture_view->desc.texture.as<Texture>();
    std::erase(texture->views, id);

    m_texture_views.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::TextureView,
        .texture_view_id = id,
        .generation = 0,
    });
}

ResourceHandle RenderServer::get_texture_view_handle(const TextureViewId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_texture_views.contains(id));

    const TextureView *texture_view = id.as<TextureView>();
    const Texture *texture = texture_view->desc.texture.as<Texture>();
    HE_ASSERT((texture->desc.usage & TextureUsage::Resource) == TextureUsage::Resource);
    HE_ASSERT(texture_view->desc.handle.has_value());

    return texture_view->desc.handle.value();
}

PipelineLayoutId RenderServer::create_pipeline_layout(const PipelineLayoutDescriptor &desc)
{
    if (desc.label.has_value())
    {
        HE_ASSERT(!desc.label.value().empty());
    }

    HE_ASSERT((desc.push_constant_size % 4) == 0);

    const PipelineLayoutId id = m_render_driver->create_pipeline_layout(desc);
    PipelineLayout *pipeline_layout = id.as<PipelineLayout>();
    pipeline_layout->desc = desc;
    m_pipeline_layouts.insert(id);
    return id;
}

void RenderServer::destroy_pipeline_layout(const PipelineLayoutId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_pipeline_layouts.contains(id));

    m_pipeline_layouts.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::PipelineLayout,
        .pipeline_layout_id = id,
        .generation = 0,
    });
}

ComputePipelineId RenderServer::create_compute_pipeline(const ComputePipelineDescriptor &desc)
{
    if (desc.label.has_value())
    {
        HE_ASSERT(!desc.label.value().empty());
    }

    HE_ASSERT(m_pipeline_layouts.contains(desc.layout));
    HE_ASSERT(m_shaders.contains(desc.shader));

    const ComputePipelineId id = m_render_driver->create_compute_pipeline(desc);
    ComputePipeline *compute_pipeline = id.as<ComputePipeline>();
    compute_pipeline->desc = desc;
    m_compute_pipelines.insert(id);
    return id;
}

void RenderServer::destroy_compute_pipeline(const ComputePipelineId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_compute_pipelines.contains(id));

    m_compute_pipelines.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::ComputePipeline,
        .compute_pipeline_id = id,
        .generation = 0,
    });
}

RenderPipelineId RenderServer::create_render_pipeline(const RenderPipelineDescriptor &desc)
{
    if (desc.label.has_value())
    {
        HE_ASSERT(!desc.label.value().empty());
    }

    HE_ASSERT(desc.layout.is_valid());
    HE_ASSERT(desc.vertex_shader.is_valid());
    HE_ASSERT(desc.fragment_shader.is_valid());
    HE_ASSERT(m_pipeline_layouts.contains(desc.layout));
    HE_ASSERT(m_shaders.contains(desc.vertex_shader));
    HE_ASSERT(m_shaders.contains(desc.fragment_shader));

    HE_ASSERT(!desc.color_attachment_states.empty());
    for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states)
    {
        HE_ASSERT(color_attachment_state.format != Format::Unknown);
    }

    if (desc.depth_stencil_state.has_value())
    {
        const DepthStencilState &depth_stencil_state = desc.depth_stencil_state.value();
        HE_ASSERT(depth_stencil_state.depth_format != Format::Unknown);
    }

    const RenderPipelineId id = m_render_driver->create_render_pipeline(desc);
    RenderPipeline *render_pipeline = id.as<RenderPipeline>();
    render_pipeline->desc = desc;
    m_render_pipelines.insert(id);
    return id;
}

void RenderServer::destroy_render_pipeline(const RenderPipelineId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_render_pipelines.contains(id));

    m_render_pipelines.erase(id);
    m_deletion_queue.push_back({
        .tag = ResourceTag::RenderPipeline,
        .render_pipeline_id = id,
        .generation = 0,
    });
}

CommandBufferId RenderServer::acquire_command_buffer()
{
    const u32 frame_id = m_frame_index % static_cast<uint32_t>(m_command_buffers.size());

    const CommandBufferId command_buffer_id = m_command_buffers[frame_id];
    CommandBuffer *command_buffer = command_buffer_id.as<CommandBuffer>();
    command_buffer->compute_pass_in_progress = false;
    command_buffer->render_pass_in_progress = false;

    m_render_driver->acquire_command_buffer(command_buffer_id);

    for (Resource &resource : m_deletion_queue)
    {
        resource.generation += 1;

        if (resource.generation == s_frames_in_flight)
        {
            switch (resource.tag)
            {
            case ResourceTag::Buffer:
            {
                const Buffer *buffer = resource.buffer_id.as<Buffer>();
                if (buffer->desc.handle.has_value())
                {
                    retire_handle(buffer->desc.handle.value());
                }
                m_render_driver->destroy_buffer(resource.buffer_id);
                break;
            }
            case ResourceTag::Shader: m_render_driver->destroy_shader(resource.shader_id); break;
            case ResourceTag::Sampler:
            {
                const Sampler *sampler = resource.sampler_id.as<Sampler>();
                if (sampler->desc.handle.has_value())
                {
                    retire_handle(sampler->desc.handle.value());
                }
                m_render_driver->destroy_sampler(resource.sampler_id);
                break;
            }
            case ResourceTag::Texture: m_render_driver->destroy_texture(resource.texture_id); break;
            case ResourceTag::TextureView:
            {
                const TextureView *texture_view = resource.texture_view_id.as<TextureView>();
                if (texture_view->desc.handle.has_value())
                {
                    retire_handle(texture_view->desc.handle.value());
                }
                m_render_driver->destroy_texture_view(resource.texture_view_id);
                break;
            }
            case ResourceTag::PipelineLayout: m_render_driver->destroy_pipeline_layout(resource.pipeline_layout_id); break;
            case ResourceTag::ComputePipeline:
                m_render_driver->destroy_compute_pipeline(resource.compute_pipeline_id);
                break;
            case ResourceTag::RenderPipeline: m_render_driver->destroy_render_pipeline(resource.render_pipeline_id); break;
            default: break;
            }
        }
    }

    std::erase_if(m_deletion_queue,
        [](const Resource &resource)
        {
            return resource.generation == s_frames_in_flight;
        });

    return command_buffer_id;
}

void RenderServer::submit_command_buffer(const CommandBufferId id)
{
    HE_ASSERT(id.is_valid());

    m_render_driver->submit_command_buffer(id);

    for (const ComputePassId compute_pass_id : m_compute_passes)
    {
        const ComputePass *compute_pass = compute_pass_id.as<ComputePass>();
        HE_ASSERT(compute_pass->ended);
        delete compute_pass;
    }

    for (const RenderPassId render_pass_id : m_render_passes)
    {
        const RenderPass *render_pass = render_pass_id.as<RenderPass>();
        HE_ASSERT(render_pass->ended);
        delete render_pass;
    }

    m_compute_passes.clear();
    m_render_passes.clear();
    m_frame_index += 1;
}

void RenderServer::write_buffer(
    const CommandBufferId id, const BufferTargetDescriptor &buffer_target, const void *data, const usize size)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_buffers.contains(buffer_target.buffer));

    const Buffer *buffer = buffer_target.buffer.as<Buffer>();
    HE_ASSERT(buffer->desc.size >= size);

    const BufferId staging_buffer = m_render_driver->create_buffer({
        .label = std::nullopt,
        .size = size,
        .usage = BufferUsage::Storage,
    });
    void *mapped_ptr = m_render_driver->map_buffer(staging_buffer);
    memcpy(mapped_ptr, data, size);
    m_render_driver->unmap_buffer(staging_buffer);

    m_render_driver->copy_buffer_to_buffer(id,
        {
            .buffer = staging_buffer,
            .offset = 0,
        },
        buffer_target, size);

    m_deletion_queue.push_back(Resource {
        .tag = ResourceTag::Buffer,
        .buffer_id = staging_buffer,
        .generation = 0,
    });
}

void RenderServer::write_texture(const CommandBufferId id,
    const TextureTargetDescriptor &texture_target,
    const void *data,
    const usize size,
    const Extent3d extent)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_textures.contains(texture_target.texture));

    // FIXME: Add check for texture size and data size

    const BufferId staging_buffer = m_render_driver->create_buffer({
        .label = std::nullopt,
        .size = size,
        .usage = BufferUsage::Storage,
    });
    void *mapped_ptr = m_render_driver->map_buffer(staging_buffer);
    memcpy(mapped_ptr, data, size);
    m_render_driver->unmap_buffer(staging_buffer);

    m_render_driver->copy_buffer_to_texture(id,
        {
            .buffer = staging_buffer,
            .offset = 0,
        },
        texture_target, extent);

    m_deletion_queue.push_back(Resource {
        .tag = ResourceTag::Buffer,
        .buffer_id = staging_buffer,
        .generation = 0,
    });
}

void RenderServer::copy_buffer_to_buffer(
    const CommandBufferId id, const BufferTargetDescriptor &src, const BufferTargetDescriptor &dst, const usize size) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_buffers.contains(src.buffer));
    HE_ASSERT(m_buffers.contains(dst.buffer));

    m_render_driver->copy_buffer_to_buffer(id, src, dst, size);
}

void RenderServer::copy_buffer_to_texture(const CommandBufferId id,
    const BufferTargetDescriptor &src,
    const TextureTargetDescriptor &dst,
    const Extent3d extent) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_buffers.contains(src.buffer));
    HE_ASSERT(m_textures.contains(dst.texture));

    m_render_driver->copy_buffer_to_texture(id, src, dst, extent);
}

void RenderServer::copy_texture_to_buffer(const CommandBufferId id,
    const TextureTargetDescriptor &src,
    const BufferTargetDescriptor &dst,
    const Extent3d extent) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_textures.contains(src.texture));
    HE_ASSERT(m_buffers.contains(dst.buffer));

    m_render_driver->copy_texture_to_buffer(id, src, dst, extent);
}

void RenderServer::copy_texture_to_texture(const CommandBufferId id,
    const TextureTargetDescriptor &src,
    const TextureTargetDescriptor &dst,
    const Extent3d extent) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_textures.contains(src.texture));
    HE_ASSERT(m_textures.contains(dst.texture));

    m_render_driver->copy_texture_to_texture(id, src, dst, extent);
}

TextureViewId RenderServer::acquire_swapchain_texture(const CommandBufferId id)
{
    HE_ASSERT(id.is_valid());

    const std::pair<TextureViewId, bool> swapchain_texture = m_render_driver->acquire_swapchain_texture(id);

    if (swapchain_texture.second)
    {
        m_swapchain_texture_views.clear();

        const std::unordered_set<TextureViewId> texture_views
            = m_render_driver->query_swapchain_texture_views() | std::ranges::to<std::unordered_set<TextureViewId>>();
        m_swapchain_texture_views = texture_views;
    }

    return swapchain_texture.first;
}

ComputePassId RenderServer::begin_compute_pass(const CommandBufferId id, const ComputePassDescriptor &desc)
{
    HE_ASSERT(id.is_valid());

    CommandBuffer *command_buffer = id.as<CommandBuffer>();
    HE_ASSERT(!command_buffer->compute_pass_in_progress);

    command_buffer->compute_pass_in_progress = true;

    ComputePass *compute_pass = new ComputePass {
        .command_buffer = id,
        .compute_pipeline = {},
        .has_label = false,
        .ended = false,
    };

    if (desc.label.has_value())
    {
        m_render_driver->begin_gpu_marker(id, desc.label.value());
        compute_pass->has_label = true;
    }

    m_render_driver->begin_compute_pass(id, desc);

    const ComputePassId compute_pass_id(compute_pass);
    m_compute_passes.push_back(compute_pass_id);

    return compute_pass_id;
}

void RenderServer::end_compute_pass(const ComputePassId id) const
{
    HE_ASSERT(id.is_valid());

    ComputePass *compute_pass = id.as<ComputePass>();
    HE_ASSERT(!compute_pass->ended);

    m_render_driver->end_compute_pass(compute_pass->command_buffer);

    if (compute_pass->has_label)
    {
        m_render_driver->end_gpu_marker(compute_pass->command_buffer);
    }

    compute_pass->ended = true;

    CommandBuffer *command_buffer = compute_pass->command_buffer.as<CommandBuffer>();
    command_buffer->compute_pass_in_progress = false;
}

void RenderServer::bind_pipeline(const ComputePassId id, const ComputePipelineId compute_pipeline) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_compute_pipelines.contains(compute_pipeline));

    ComputePass *compute_pass = id.as<ComputePass>();
    compute_pass->compute_pipeline = compute_pipeline;

    m_render_driver->bind_compute_pipeline(compute_pass->command_buffer, compute_pipeline);
}

void RenderServer::push_constants(const ComputePassId id, const void *data, const usize size) const
{
    HE_ASSERT(id.is_valid());

    const ComputePass *compute_pass = id.as<ComputePass>();
    HE_ASSERT(!compute_pass->ended);
    HE_ASSERT(compute_pass->compute_pipeline.is_valid());

    const ComputePipeline *compute_pipeline = compute_pass->compute_pipeline.as<ComputePipeline>();
    const PipelineLayout *pipeline_layout = compute_pipeline->desc.layout.as<PipelineLayout>();
    HE_ASSERT(pipeline_layout->desc.push_constant_size == size);

    m_render_driver->push_constants(compute_pass->command_buffer, compute_pipeline->desc.layout, data, size);
}

void RenderServer::dispatch(const ComputePassId id, const u32 x, const u32 y, const u32 z) const
{
    HE_ASSERT(id.is_valid());

    const ComputePass *compute_pass = id.as<ComputePass>();
    HE_ASSERT(!compute_pass->ended);
    HE_ASSERT(compute_pass->compute_pipeline.is_valid());

    m_render_driver->dispatch(compute_pass->command_buffer, x, y, z);
}

RenderPassId RenderServer::begin_render_pass(const CommandBufferId id, const RenderPassDescriptor &desc)
{
    HE_ASSERT(id.is_valid());

    HE_ASSERT(!desc.color_attachments.empty());
    for (const ColorAttachment &color_attachment : desc.color_attachments)
    {
        HE_ASSERT(
            m_texture_views.contains(color_attachment.view) || m_swapchain_texture_views.contains(color_attachment.view));
    }

    if (desc.depth_stencil_attachment.has_value())
    {
        HE_ASSERT(m_texture_views.contains(desc.depth_stencil_attachment.value().view));
    }

    CommandBuffer *command_buffer = id.as<CommandBuffer>();
    HE_ASSERT(!command_buffer->render_pass_in_progress);

    command_buffer->render_pass_in_progress = true;

    RenderPass *render_pass = new RenderPass {
        .command_buffer = id,
        .render_pipeline = {},
        .has_label = false,
        .ended = false,
    };

    if (desc.label.has_value())
    {
        m_render_driver->begin_gpu_marker(id, desc.label.value());
        render_pass->has_label = true;
    }

    m_render_driver->begin_render_pass(id, desc);

    const RenderPassId render_pass_id(render_pass);
    m_render_passes.push_back(render_pass_id);

    return render_pass_id;
}

void RenderServer::end_render_pass(const RenderPassId id) const
{
    HE_ASSERT(id.is_valid());

    RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);

    m_render_driver->end_render_pass(render_pass->command_buffer);

    if (render_pass->has_label)
    {
        m_render_driver->end_gpu_marker(render_pass->command_buffer);
    }

    render_pass->ended = true;

    CommandBuffer *command_buffer = render_pass->command_buffer.as<CommandBuffer>();
    command_buffer->render_pass_in_progress = false;
}

void RenderServer::bind_pipeline(const RenderPassId id, const RenderPipelineId render_pipeline) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_render_pipelines.contains(render_pipeline));

    RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);

    render_pass->render_pipeline = render_pipeline;

    m_render_driver->bind_render_pipeline(render_pass->command_buffer, render_pipeline);
}

void RenderServer::bind_index_buffer(const RenderPassId id, const BufferId buffer) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_buffers.contains(buffer));

    const RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);

    m_render_driver->bind_index_buffer(render_pass->command_buffer, buffer);
}

void RenderServer::push_constants(const RenderPassId id, const void *data, const usize size) const
{
    HE_ASSERT(id.is_valid());

    const RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);
    HE_ASSERT(render_pass->render_pipeline.is_valid());

    const RenderPipeline *render_pipeline = render_pass->render_pipeline.as<RenderPipeline>();
    const PipelineLayout *pipeline_layout = render_pipeline->desc.layout.as<PipelineLayout>();
    HE_ASSERT(pipeline_layout->desc.push_constant_size == size);

    m_render_driver->push_constants(render_pass->command_buffer, render_pipeline->desc.layout, data, size);
}

void RenderServer::set_viewport(const RenderPassId id,
    const f32 x,
    const f32 y,
    const f32 width,
    const f32 height,
    const f32 min_depth,
    const f32 max_depth) const
{
    HE_ASSERT(id.is_valid());

    const RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);

    m_render_driver->set_viewport(render_pass->command_buffer, x, y, width, height, min_depth, max_depth);
}

void RenderServer::set_scissor(const RenderPassId id, const Offset2d offset, const Extent2d extent) const
{
    HE_ASSERT(id.is_valid());

    const RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);

    m_render_driver->set_scissor(render_pass->command_buffer, offset, extent);
}

void RenderServer::draw(const RenderPassId id,
    const u32 vertex_count,
    const u32 instance_count,
    const u32 first_vertex,
    const u32 first_instance) const
{
    HE_ASSERT(id.is_valid());

    const RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);

    m_render_driver->draw(render_pass->command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

void RenderServer::draw_indexed(const RenderPassId id,
    const u32 index_count,
    const u32 instance_count,
    const u32 first_index,
    const i32 vertex_offset,
    const u32 first_instance) const
{
    HE_ASSERT(id.is_valid());

    const RenderPass *render_pass = id.as<RenderPass>();
    HE_ASSERT(!render_pass->ended);

    m_render_driver->draw_indexed(
        render_pass->command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
}

ResourceHandle RenderServer::allocate_handle()
{
    if (m_recycled_descriptors.empty())
    {
        return ResourceHandle(m_current_descriptor_index++);
    }

    const ResourceHandle handle = m_recycled_descriptors.top();
    m_recycled_descriptors.pop();
    return handle;
}

void RenderServer::retire_handle(const ResourceHandle handle)
{
    if (handle.is_valid())
    {
        return;
    }

    m_recycled_descriptors.push(handle);
}

void RenderServer::on_resize(const WindowResizeEvent &event) const { m_render_driver->on_resize(event); }
