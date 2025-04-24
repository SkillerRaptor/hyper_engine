/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "engine.hpp"

#include <chrono>

#include "core/logger.hpp"
#include "shader_interop.h"
#include "systems/window/window_events.hpp"

void Engine::initialize()
{
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    Logger::initialize();

    m_window_system = new WindowSystem();
    HE_ASSERT(m_window_system != nullptr);
    m_window_system->initialize();

    const WindowDescriptor window_descriptor = {
        .title = "HyperEngine",
        .width = 1280,
        .height = 720,
    };
    m_window = m_window_system->create_window(window_descriptor);

    m_input_system = new InputSystem();
    HE_ASSERT(m_input_system != nullptr);
    m_input_system->initialize(*m_window_system);

    m_render_system = new RenderSystem();
    HE_ASSERT(m_render_system != nullptr);
    m_render_system->initialize(*m_window_system, m_window);

    // Camera
    m_window_system->register_listener<MouseMoveEvent>(
        [this](const MouseMoveEvent &event)
        {
            m_camera.process_mouse_movement(event.x(), event.y(), m_input_system->is_mouse_button_pressed(MouseCode::ButtonMiddle));
        });
    m_window_system->register_listener<MouseScrollEvent>(
        [this](const MouseScrollEvent &event)
        {
            m_camera.process_mouse_scroll(event.delta_y());
        });

    const BufferDescriptor camera_buffer_descriptor = {
        .label = std::nullopt,
        .size = sizeof(ShaderCamera),
        .usage =
            {
                BufferUsage::Storage,
                BufferUsage::ShaderResource,
            },
        .handle = ResourceHandle(HE_DESCRIPTOR_SET_SLOT_CAMERA),
    };
    m_camera_buffer = m_render_system->create_buffer(camera_buffer_descriptor);

    // Rendering
    const PipelineLayoutDescriptor pipeline_layout_descriptor = {
        .label = std::nullopt,
        .push_constant_size = 0,
    };
    m_pipeline_layout = m_render_system->create_pipeline_layout(pipeline_layout_descriptor);

    const ShaderDescriptor vertex_shader_descriptor = {
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/triangle_shader.hlsl",
    };
    const ShaderId vertex_shader = m_render_system->create_shader(vertex_shader_descriptor);

    const ShaderDescriptor fragment_shader_descriptor = {
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/triangle_shader.hlsl",
    };
    const ShaderId fragment_shader = m_render_system->create_shader(fragment_shader_descriptor);

    const RenderPipelineDescriptor pipeline_descriptor = {
        .label = std::nullopt,
        .layout = m_pipeline_layout,
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .color_attachment_states =
            {
                {
                    .format = Format::Bgra8Unorm,
                    .blend_state =
                        {
                            .blend_enable = false,
                            .src_blend_factor = BlendFactor::One,
                            .dst_blend_factor = BlendFactor::Zero,
                            .operation = BlendOperation::Add,
                            .alpha_src_blend_factor = BlendFactor::One,
                            .alpha_dst_blend_factor = BlendFactor::Zero,
                            .alpha_operation = BlendOperation::Add,
                            .color_writes = ColorWrites::All,
                        },
                },
            },
        .primitive_state =
            {
                .topology = PrimitiveTopology::TriangleList,
                .front_face = FrontFace::CounterClockwise,
                .cull_mode = Face::None,
                .polygon_mode = PolygonMode::Fill,
            },
        .depth_stencil_state =
            {
                .depth_test_enable = true,
                .depth_write_enable = true,
                .depth_format = Format::D32Sfloat,
                .depth_compare_operation = CompareOperation::Less,
                .depth_bias_state =
                    {
                        .depth_bias_enable = false,
                        .constant = 0.0f,
                        .clamp = 0.0f,
                        .slope = 0.0f,
                    },
            },
    };
    m_render_pipeline = m_render_system->create_render_pipeline(pipeline_descriptor);

    const ShaderDescriptor grid_vertex_shader_descriptor = {
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/grid_shader.hlsl",
    };
    const ShaderId grid_vertex_shader = m_render_system->create_shader(grid_vertex_shader_descriptor);

    const ShaderDescriptor grid_fragment_shader_descriptor = {
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/grid_shader.hlsl",
    };
    const ShaderId grid_fragment_shader = m_render_system->create_shader(grid_fragment_shader_descriptor);

    const RenderPipelineDescriptor grid_pipeline_descriptor = {
        .label = std::nullopt,
        .layout = m_pipeline_layout,
        .vertex_shader = grid_vertex_shader,
        .fragment_shader = grid_fragment_shader,
        .color_attachment_states =
            {
                {
                    .format = Format::Bgra8Unorm,
                    .blend_state =
                        {
                            .blend_enable = true,
                            .src_blend_factor = BlendFactor::SrcAlpha,
                            .dst_blend_factor = BlendFactor::One,
                            .operation = BlendOperation::Add,
                            .alpha_src_blend_factor = BlendFactor::One,
                            .alpha_dst_blend_factor = BlendFactor::Zero,
                            .alpha_operation = BlendOperation::Add,
                            .color_writes = ColorWrites::All,
                        },
                },
            },
        .primitive_state =
            {
                .topology = PrimitiveTopology::TriangleList,
                .front_face = FrontFace::CounterClockwise,
                .cull_mode = Face::None,
                .polygon_mode = PolygonMode::Fill,
            },
        .depth_stencil_state =
            {
                .depth_test_enable = true,
                .depth_write_enable = true,
                .depth_format = Format::D32Sfloat,
                .depth_compare_operation = CompareOperation::Less,
                .depth_bias_state =
                    {
                        .depth_bias_enable = false,
                        .constant = 0.0f,
                        .clamp = 0.0f,
                        .slope = 0.0f,
                    },
            },
    };
    m_grid_pipeline = m_render_system->create_render_pipeline(grid_pipeline_descriptor);

    m_render_system->destroy_shader(grid_fragment_shader);
    m_render_system->destroy_shader(grid_vertex_shader);

    const glm::uvec2 window_size = m_window_system->get_window_size(m_window);
    const TextureDescriptor depth_texture_descriptor = {
        .label = std::nullopt,
        .width = window_size.x,
        .height = window_size.y,
        .depth = 1,
        .array_size = 1,
        .mip_levels = 1,
        .format = Format::D32Sfloat,
        .dimension = Dimension::Texture2D,
        .usage = TextureUsage::RenderAttachment,
    };
    m_depth_texture = m_render_system->create_texture(depth_texture_descriptor);

    m_window_system->register_listener<WindowResizeEvent>(
        [this](const WindowResizeEvent &event)
        {
            m_render_system->destroy_texture(m_depth_texture);

            const TextureDescriptor new_depth_texture_descriptor = {
                .label = std::nullopt,
                .width = event.width(),
                .height = event.height(),
                .depth = 1,
                .array_size = 1,
                .mip_levels = 1,
                .format = Format::D32Sfloat,
                .dimension = Dimension::Texture2D,
                .usage = TextureUsage::RenderAttachment,
            };

            m_depth_texture = m_render_system->create_texture(new_depth_texture_descriptor);
        });

    m_render_system->destroy_shader(fragment_shader);
    m_render_system->destroy_shader(vertex_shader);

    m_window_system->register_listener<WindowCloseEvent>(
        [this](const WindowCloseEvent &)
        {
            m_running = false;
        });

    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    HE_INFO("Engine initialized in {:.2}s", elapsed_seconds.count());
}

void Engine::shutdown() const
{
    m_render_system->destroy_texture(m_depth_texture);
    m_render_system->destroy_render_pipeline(m_render_pipeline);
    m_render_system->destroy_render_pipeline(m_grid_pipeline);
    m_render_system->destroy_pipeline_layout(m_pipeline_layout);
    m_render_system->destroy_buffer(m_camera_buffer);

    m_window_system->destroy_window(m_window);
}

void Engine::run()
{
    float total_time = 0.0;
    constexpr float delta_time = 1.0f / 60.0f;

    float accumulator = 0.0;
    std::chrono::time_point current_time = std::chrono::steady_clock::now();
    while (m_running)
    {
        const std::chrono::time_point new_time = std::chrono::steady_clock::now();
        const float frame_time = std::chrono::duration<float>(new_time - current_time).count();
        current_time = new_time;

        accumulator += frame_time;

        m_window_system->poll_events();

        while (accumulator >= delta_time)
        {
            // Fixed Update
            fixed_update(delta_time);

            accumulator -= delta_time;
            total_time += delta_time;
        }

        // Update
        update(delta_time);

        // Render
        render();
    }
}

void Engine::fixed_update(const float delta_time)
{
    (void) delta_time;
}

void Engine::update(const float delta_time)
{
    if (m_input_system->is_key_pressed(KeyCode::W))
    {
        m_camera.process_keyboard(Camera::Movement::Forward, delta_time);
    }

    if (m_input_system->is_key_pressed(KeyCode::S))
    {
        m_camera.process_keyboard(Camera::Movement::Backward, delta_time);
    }

    if (m_input_system->is_key_pressed(KeyCode::A))
    {
        m_camera.process_keyboard(Camera::Movement::Left, delta_time);
    }

    if (m_input_system->is_key_pressed(KeyCode::D))
    {
        m_camera.process_keyboard(Camera::Movement::Right, delta_time);
    }
}

void Engine::render() const
{
    const CommandBufferId command_buffer = m_render_system->acquire_command_buffer();

    const glm::mat4 view_matrix = m_camera.view_matrix();
    const glm::mat4 projection_matrix = m_camera.projection_matrix();
    const glm::mat4 view_projection_matrix = projection_matrix * view_matrix;
    const ShaderCamera shader_camera = {
        .position = glm::vec4(m_camera.position(), 1.0),
        .view = view_matrix,
        .inverse_view = glm::inverse(view_matrix),
        .projection = projection_matrix,
        .inverse_projection = glm::inverse(projection_matrix),
        .view_projection = view_projection_matrix,
        .inverse_view_projection = glm::inverse(view_projection_matrix),
        .near_plane = m_camera.near_plane(),
        .far_plane = m_camera.far_plane(),
        .padding_0 = 0.0,
        .padding_1 = 0.0,
    };
    m_render_system->write_buffer(command_buffer, m_camera_buffer, shader_camera, 0);

    const TextureId swapchain_texture = m_render_system->acquire_swapchain_texture(command_buffer);

    const RenderPassDescriptor render_pass_descriptor = {
        .label =
            Label{
                .name = "Render Pass",
                .color =
                    {
                        .r = 255,
                        .g = 0,
                        .b = 0,
                    },
            },
        .color_attachments =
            {
                {
                    .texture = swapchain_texture,
                    .operations =
                        {
                            .load_operation = LoadOperation::Clear,
                            .store_operation = StoreOperation::Store,
                        },
                },
            },
        .depth_stencil_attachment =
            DepthStencilAttachment{
                .texture = m_depth_texture,
                .depth_operations =
                    {
                        .load_operation = LoadOperation::Clear,
                        .store_operation = StoreOperation::Store,
                    },
            },
    };

    const RenderPassId render_pass = m_render_system->begin_render_pass(command_buffer, render_pass_descriptor);
    m_render_system->bind_pipeline(render_pass, m_render_pipeline);
    m_render_system->draw(render_pass, 3, 1, 0, 0);
    m_render_system->end_render_pass(render_pass);

    const RenderPassDescriptor grid_render_pass_descriptor = {
        .label =
            Label{
                .name = "Grid Render Pass",
                .color =
                    {
                        .r = 0,
                        .g = 255,
                        .b = 0,
                    },
            },
        .color_attachments =
            {
                {
                    .texture = swapchain_texture,
                    .operations =
                        {
                            .load_operation = LoadOperation::Load,
                            .store_operation = StoreOperation::Store,
                        },
                },
            },
        .depth_stencil_attachment =
            DepthStencilAttachment{
                .texture = m_depth_texture,
                .depth_operations =
                    {
                        .load_operation = LoadOperation::Load,
                        .store_operation = StoreOperation::Store,
                    },
            },
    };

    const RenderPassId grid_render_pass = m_render_system->begin_render_pass(command_buffer, grid_render_pass_descriptor);
    m_render_system->bind_pipeline(grid_render_pass, m_grid_pipeline);
    m_render_system->draw(grid_render_pass, 6, 1, 0, 0);
    m_render_system->end_render_pass(grid_render_pass);

    m_render_system->submit_command_buffer(command_buffer);
}