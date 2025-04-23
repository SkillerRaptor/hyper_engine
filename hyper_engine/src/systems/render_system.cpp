/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

// FIXME: The destroy function doesn't queue the resources so it's not ensured that they are safe to be destroyed

#include "systems/render_system.hpp"

#include "core/assertion.hpp"
#include "core/logger.hpp"
#include "drivers/vulkan/vulkan_render_driver.hpp"

RenderSystem::~RenderSystem()
{
    for (uint8_t i = 0; i < s_frames_in_flight; ++i)
    {
        m_render_driver->destroy_command_buffer(m_command_buffers[i]);
    }

    delete m_render_driver;
}

void RenderSystem::initialize(const WindowSystem &window_system, const WindowId window)
{
    // FIXME: Add selection to change render driver
    m_render_driver = new VulkanRenderDriver();
    m_render_driver->initialize(window_system, window);

    const std::vector<Texture *> textures = m_render_driver->query_swapchain_textures();
    for (size_t i = 0; i < textures.size(); ++i)
    {
        Texture *texture = textures[i];
        const TextureId texture_id = m_textures.create(texture);
        m_swapchain_textures.insert({static_cast<uint32_t>(i), texture_id});
    }

    for (uint8_t i = 0; i < s_frames_in_flight; ++i)
    {
        CommandBuffer *command_buffer = m_render_driver->create_command_buffer();
        command_buffer->generation = 0;
        command_buffer->compute_pass_in_progress = false;
        command_buffer->render_pass_in_progress = false;
        command_buffer->swapchain_texture_acquired = false;

        m_command_buffers[i] = command_buffer;
    }

    HE_INFO("Successfully initialized RenderSystem");
}

BufferId RenderSystem::create_buffer(const BufferDescriptor &descriptor)
{
    HE_ASSERT(descriptor.size > 0);
    HE_ASSERT(descriptor.usage != BufferUsage::None);

    Buffer *buffer = m_render_driver->create_buffer(descriptor.label, descriptor.size, descriptor.usage, false);
    buffer->label = descriptor.label;
    buffer->size = descriptor.size;
    buffer->usage = descriptor.usage;

    return m_buffers.create(buffer);
}

void RenderSystem::destroy_buffer(const BufferId id)
{
    HE_ASSERT(m_buffers.contains(id));

    const Buffer *buffer = m_buffers.get(id);
    m_render_driver->destroy_buffer(buffer);
    m_buffers.destroy(id);
}

ShaderId RenderSystem::create_shader(const ShaderDescriptor &descriptor)
{
    HE_ASSERT(descriptor.type != ShaderType::None);
    HE_ASSERT(!descriptor.entry.empty());
    HE_ASSERT(!descriptor.path.empty());

    Shader *shader = m_render_driver->create_shader(descriptor.label, descriptor.type, descriptor.entry, descriptor.path);
    shader->label = descriptor.label;
    shader->type = descriptor.type;
    shader->entry = descriptor.entry;
    shader->path = descriptor.path;

    return m_shaders.create(shader);
}

void RenderSystem::destroy_shader(const ShaderId id)
{
    HE_ASSERT(m_shaders.contains(id));

    const Shader *shader = m_shaders.get(id);
    m_render_driver->destroy_shader(shader);
    m_shaders.destroy(id);
}

SamplerId RenderSystem::create_sampler(const SamplerDescriptor &descriptor)
{
    // FIXME: Add assertions

    Sampler *sampler = m_render_driver->create_sampler(
        descriptor.label,
        descriptor.mag_filter,
        descriptor.min_filter,
        descriptor.mipmap_filter,
        descriptor.address_mode_u,
        descriptor.address_mode_v,
        descriptor.address_mode_w,
        descriptor.mip_lod_bias,
        descriptor.compare_operation,
        descriptor.min_lod,
        descriptor.max_lod,
        descriptor.border_color);
    sampler->label = descriptor.label;
    sampler->mag_filter = descriptor.mag_filter;
    sampler->min_filter = descriptor.min_filter;
    sampler->mipmap_filter = descriptor.mipmap_filter;
    sampler->address_mode_u = descriptor.address_mode_u;
    sampler->address_mode_v = descriptor.address_mode_v;
    sampler->address_mode_w = descriptor.address_mode_w;
    sampler->mip_lod_bias = descriptor.mip_lod_bias;
    sampler->compare_operation = descriptor.compare_operation;
    sampler->min_lod = descriptor.min_lod;
    sampler->max_lod = descriptor.max_lod;
    sampler->border_color = descriptor.border_color;

    return m_samplers.create(sampler);
}

void RenderSystem::destroy_sampler(const SamplerId id)
{
    HE_ASSERT(m_samplers.contains(id));

    const Sampler *sampler = m_samplers.get(id);
    m_render_driver->destroy_sampler(sampler);
    m_samplers.destroy(id);
}

TextureId RenderSystem::create_texture(const TextureDescriptor &descriptor)
{
    HE_ASSERT(descriptor.width > 0);
    HE_ASSERT(descriptor.height > 0);
    HE_ASSERT(descriptor.depth > 0);
    HE_ASSERT(descriptor.array_size > 0);
    HE_ASSERT(descriptor.mip_levels > 0);
    HE_ASSERT(descriptor.format != Format::Unknown);
    HE_ASSERT(descriptor.dimension != Dimension::Unknown);
    HE_ASSERT(descriptor.usage != TextureUsage::None);

    Texture *texture = m_render_driver->create_texture(
        descriptor.label,
        descriptor.width,
        descriptor.height,
        descriptor.depth,
        descriptor.array_size,
        descriptor.mip_levels,
        descriptor.format,
        descriptor.dimension,
        descriptor.usage);
    texture->label = descriptor.label;
    texture->width = descriptor.width;
    texture->height = descriptor.height;
    texture->depth = descriptor.depth;
    texture->array_size = descriptor.array_size;
    texture->mip_levels = descriptor.mip_levels;
    texture->format = descriptor.format;
    texture->dimension = descriptor.dimension;
    texture->usage = descriptor.usage;

    return m_textures.create(texture);
}

void RenderSystem::destroy_texture(const TextureId id)
{
    HE_ASSERT(m_textures.contains(id));

    const Texture *texture = m_textures.get(id);
    m_render_driver->destroy_texture(texture);
    m_textures.destroy(id);
}

PipelineLayoutId RenderSystem::create_pipeline_layout(const PipelineLayoutDescriptor &descriptor)
{
    HE_ASSERT((descriptor.push_constant_size % 4) == 0);

    PipelineLayout *pipeline_layout = m_render_driver->create_pipeline_layout(descriptor.label, descriptor.push_constant_size);
    pipeline_layout->label = descriptor.label;
    pipeline_layout->push_constant_size = descriptor.push_constant_size;

    return m_pipeline_layouts.create(pipeline_layout);
}

void RenderSystem::destroy_pipeline_layout(const PipelineLayoutId id)
{
    HE_ASSERT(m_pipeline_layouts.contains(id));

    const PipelineLayout *pipeline_layout = m_pipeline_layouts.get(id);
    m_render_driver->destroy_pipeline_layout(pipeline_layout);
    m_pipeline_layouts.destroy(id);
}

ComputePipelineId RenderSystem::create_compute_pipeline(const ComputePipelineDescriptor &descriptor)
{
    HE_ASSERT(m_pipeline_layouts.contains(descriptor.layout));
    HE_ASSERT(m_shaders.contains(descriptor.shader));

    PipelineLayout *layout = m_pipeline_layouts.get(descriptor.layout);
    Shader *shader = m_shaders.get(descriptor.shader);

    ComputePipeline *compute_pipeline = m_render_driver->create_compute_pipeline(descriptor.label, layout, shader);
    compute_pipeline->label = descriptor.label;
    compute_pipeline->layout = layout;
    compute_pipeline->shader = shader;

    return m_compute_pipelines.create(compute_pipeline);
}

void RenderSystem::destroy_compute_pipeline(const ComputePipelineId id)
{
    HE_ASSERT(m_compute_pipelines.contains(id));

    const ComputePipeline *compute_pipeline = m_compute_pipelines.get(id);
    m_render_driver->destroy_compute_pipeline(compute_pipeline);
    m_compute_pipelines.destroy(id);
}

RenderPipelineId RenderSystem::create_render_pipeline(const RenderPipelineDescriptor &descriptor)
{
    HE_ASSERT(m_pipeline_layouts.contains(descriptor.layout));
    HE_ASSERT(m_shaders.contains(descriptor.vertex_shader));
    HE_ASSERT(m_shaders.contains(descriptor.fragment_shader));
    HE_ASSERT(!descriptor.color_attachment_states.empty());

    for (const ColorAttachmentState &color_attachment_state : descriptor.color_attachment_states)
    {
        HE_ASSERT(color_attachment_state.format != Format::Unknown);
    }

    if (descriptor.depth_stencil_state.depth_test_enable)
    {
        HE_ASSERT(descriptor.depth_stencil_state.depth_format != Format::Unknown);
    }

    PipelineLayout *layout = m_pipeline_layouts.get(descriptor.layout);
    Shader *vertex_shader = m_shaders.get(descriptor.vertex_shader);
    Shader *fragment_shader = m_shaders.get(descriptor.fragment_shader);

    RenderPipeline *render_pipeline = m_render_driver->create_render_pipeline(
        descriptor.label,
        layout,
        vertex_shader,
        fragment_shader,
        descriptor.color_attachment_states,
        descriptor.primitive_state,
        descriptor.depth_stencil_state);
    render_pipeline->label = descriptor.label;
    render_pipeline->layout = layout;
    render_pipeline->vertex_shader = vertex_shader;
    render_pipeline->fragment_shader = fragment_shader;
    render_pipeline->color_attachment_states = descriptor.color_attachment_states;
    render_pipeline->primitive_state = descriptor.primitive_state;
    render_pipeline->depth_stencil_state = descriptor.depth_stencil_state;

    return m_render_pipelines.create(render_pipeline);
}

void RenderSystem::destroy_render_pipeline(const RenderPipelineId id)
{
    HE_ASSERT(m_render_pipelines.contains(id));

    const RenderPipeline *render_pipeline = m_render_pipelines.get(id);
    m_render_driver->destroy_render_pipeline(render_pipeline);
    m_render_pipelines.destroy(id);
}

CommandBufferId RenderSystem::acquire_command_buffer() const
{
    const uint32_t command_buffer_id = m_frame_index % static_cast<uint32_t>(m_command_buffers.size());

    CommandBuffer *command_buffer = m_command_buffers[command_buffer_id];
    command_buffer->generation += 1;
    command_buffer->compute_pass_in_progress = false;
    command_buffer->render_pass_in_progress = false;
    command_buffer->swapchain_texture_acquired = false;

    m_render_driver->acquire_command_buffer(command_buffer);

    return CommandBufferId(command_buffer_id, command_buffer->generation);
}

void RenderSystem::submit_command_buffer(const CommandBufferId id)
{
    CommandBuffer *command_buffer = resolve_command_buffer(id);

    m_render_driver->submit_command_buffer(command_buffer);

    if (command_buffer->swapchain_texture_acquired)
    {
        m_render_driver->present();
    }

    for (const ComputePass *compute_pass : m_compute_passes)
    {
        delete compute_pass;
    }

    for (const RenderPass *render_pass : m_render_passes)
    {
        delete render_pass;
    }

    m_compute_passes.clear();
    m_render_passes.clear();
    m_frame_index += 1;
}

TextureId RenderSystem::acquire_swapchain_texture(const CommandBufferId id)
{
    CommandBuffer *command_buffer = resolve_command_buffer(id);
    command_buffer->swapchain_texture_acquired = true;

    const uint32_t swapchain_texture_index = m_render_driver->acquire_swapchain_texture(command_buffer);
    return m_swapchain_textures[swapchain_texture_index];
}

ComputePassId RenderSystem::begin_compute_pass(const CommandBufferId id, const ComputePassDescriptor &descriptor)
{
    CommandBuffer *command_buffer = resolve_command_buffer(id);
    HE_ASSERT(!command_buffer->compute_pass_in_progress);

    ComputePass *compute_pass = new ComputePass{
        .command_buffer = command_buffer,
    };

    if (descriptor.label.has_value())
    {
        const Label label = descriptor.label.value();
        m_render_driver->begin_gpu_marker(command_buffer, label);

        compute_pass->has_label = true;
    }

    m_render_driver->begin_compute_pass(command_buffer);

    command_buffer->compute_pass_in_progress = true;
    m_compute_passes.push_back(compute_pass);

    return ComputePassId(static_cast<uint32_t>(m_compute_passes.size()) - 1, command_buffer->generation);
}

void RenderSystem::end_compute_pass(const ComputePassId id) const
{
    ComputePass *compute_pass = resolve_compute_pass(id);

    m_render_driver->end_compute_pass(compute_pass->command_buffer);

    if (compute_pass->has_label)
    {
        m_render_driver->end_gpu_marker(compute_pass->command_buffer);
    }

    compute_pass->ended = true;
    compute_pass->command_buffer->render_pass_in_progress = false;
}

void RenderSystem::bind_compute_pipeline(const ComputePassId id, const ComputePipelineId pipeline_id)
{
    HE_ASSERT(m_compute_pipelines.contains(pipeline_id));

    const ComputePass *compute_pass = resolve_compute_pass(id);

    const ComputePipeline *compute_pipeline = m_compute_pipelines.get(pipeline_id);
    m_render_driver->bind_compute_pipeline(compute_pass->command_buffer, compute_pipeline);
}

RenderPassId RenderSystem::begin_render_pass(const CommandBufferId id, const RenderPassDescriptor &descriptor)
{
    HE_ASSERT(m_textures.contains(descriptor.texture));

    CommandBuffer *command_buffer = resolve_command_buffer(id);
    HE_ASSERT(!command_buffer->render_pass_in_progress);

    RenderPass *render_pass = new RenderPass{
        .command_buffer = command_buffer,
    };

    if (descriptor.label.has_value())
    {
        const Label label = descriptor.label.value();
        m_render_driver->begin_gpu_marker(command_buffer, label);

        render_pass->has_label = true;
    }

    const Texture *texture = m_textures.get(descriptor.texture);
    m_render_driver->begin_render_pass(command_buffer, texture);

    command_buffer->render_pass_in_progress = true;
    m_render_passes.push_back(render_pass);

    return RenderPassId(static_cast<uint32_t>(m_render_passes.size()) - 1, command_buffer->generation);
}

void RenderSystem::end_render_pass(const RenderPassId id) const
{
    RenderPass *render_pass = resolve_render_pass(id);

    m_render_driver->end_render_pass(render_pass->command_buffer);

    if (render_pass->has_label)
    {
        m_render_driver->end_gpu_marker(render_pass->command_buffer);
    }

    render_pass->ended = true;
    render_pass->command_buffer->render_pass_in_progress = false;
}

void RenderSystem::bind_render_pipeline(const RenderPassId id, const RenderPipelineId pipeline_id)
{
    HE_ASSERT(m_render_pipelines.contains(pipeline_id));

    const RenderPass *render_pass = resolve_render_pass(id);

    const RenderPipeline *render_pipeline = m_render_pipelines.get(pipeline_id);
    m_render_driver->bind_render_pipeline(render_pass->command_buffer, render_pipeline);
}

void RenderSystem::set_viewport(
    const RenderPassId id,
    const float x,
    const float y,
    const float width,
    const float height,
    const float min_depth,
    const float max_depth) const
{
    const RenderPass *render_pass = resolve_render_pass(id);

    m_render_driver->set_viewport(render_pass->command_buffer, x, y, width, height, min_depth, max_depth);
}

void RenderSystem::set_scissor(const RenderPassId id, const int32_t x, const int32_t y, const uint32_t width, const uint32_t height) const
{
    const RenderPass *render_pass = resolve_render_pass(id);

    m_render_driver->set_scissor(render_pass->command_buffer, x, y, width, height);
}

void RenderSystem::draw(
    const RenderPassId id,
    const uint32_t vertex_count,
    const uint32_t instance_count,
    const uint32_t first_vertex,
    const uint32_t first_instance) const
{
    const RenderPass *render_pass = resolve_render_pass(id);

    m_render_driver->draw(render_pass->command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

CommandBuffer *RenderSystem::resolve_command_buffer(const CommandBufferId id) const
{
    HE_ASSERT(id.id() < m_command_buffers.size());

    CommandBuffer *command_buffer = m_command_buffers[id.id()];
    HE_ASSERT(id.version() == command_buffer->generation);

    return command_buffer;
}

ComputePass *RenderSystem::resolve_compute_pass(const ComputePassId id) const
{
    HE_ASSERT(id.id() < m_compute_passes.size());

    ComputePass *compute_pass = m_compute_passes[id.id()];
    HE_ASSERT(id.version() == compute_pass->command_buffer->generation);
    HE_ASSERT(!compute_pass->ended);

    return compute_pass;
}

RenderPass *RenderSystem::resolve_render_pass(const RenderPassId id) const
{
    HE_ASSERT(id.id() < m_render_passes.size());

    RenderPass *render_pass = m_render_passes[id.id()];
    HE_ASSERT(id.version() == render_pass->command_buffer->generation);
    HE_ASSERT(!render_pass->ended);

    return render_pass;
}