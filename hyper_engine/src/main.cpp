/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <chrono>

#include "core/logger.hpp"
#include "systems/input_system.hpp"
#include "systems/render_system.hpp"
#include "systems/window/window_events.hpp"
#include "systems/window_system.hpp"

int main()
{
    // TODO: Parse command line arguments

    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    Logger::initialize();

    std::unique_ptr<WindowSystem> window_system = std::make_unique<WindowSystem>();
    window_system->initialize();

    const WindowDescriptor window_descriptor = {
        .title = "HyperEngine",
        .width = 1280,
        .height = 720,
    };
    const WindowId window = window_system->create_window(window_descriptor);

    std::unique_ptr<InputSystem> input_system = std::make_unique<InputSystem>();
    input_system->initialize(*window_system);

    std::unique_ptr<RenderSystem> render_system = std::make_unique<RenderSystem>();
    render_system->initialize(*window_system, window);

    const ShaderDescriptor vertex_shader_descriptor = {
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/triangle_shader.hlsl",
    };
    const ShaderId vertex_shader = render_system->create_shader(vertex_shader_descriptor);

    const ShaderDescriptor fragment_shader_descriptor = {
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/triangle_shader.hlsl",
    };
    const ShaderId fragment_shader = render_system->create_shader(fragment_shader_descriptor);

    const PipelineLayoutDescriptor pipeline_layout_descriptor = {
        .label = std::nullopt,
        .push_constant_size = 0,
    };
    const PipelineLayoutId pipeline_layout = render_system->create_pipeline_layout(pipeline_layout_descriptor);

    const RenderPipelineDescriptor pipeline_descriptor = {
        .label = std::nullopt,
        .layout = pipeline_layout,
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
                .cull_mode = Face::Back,
                .polygon_mode = PolygonMode::Fill,
            },
        .depth_stencil_state =
            {
                .depth_test_enable = false,
                .depth_write_enable = false,
                .depth_format = Format::Unknown,
                .depth_compare_operation = CompareOperation::Never,
                .depth_bias_state =
                    {
                        .depth_bias_enable = false,
                        .constant = 0.0f,
                        .clamp = 0.0f,
                        .slope = 0.0f,
                    },
            },
    };
    const RenderPipelineId pipeline = render_system->create_render_pipeline(pipeline_descriptor);

    render_system->destroy_shader(fragment_shader);
    render_system->destroy_shader(vertex_shader);

    bool running = true;
    window_system->register_listener<WindowCloseEvent>(
        [&running](const WindowCloseEvent &)
        {
            running = false;
        });

    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    HE_INFO("Engine initialized in {:.2}s", elapsed_seconds.count());

    while (running)
    {
        window_system->poll_events();

        const CommandBufferId command_buffer = render_system->acquire_command_buffer();
        const TextureId swapchain_texture = render_system->acquire_swapchain_texture(command_buffer);

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
            .texture = swapchain_texture,
        };

        const RenderPassId render_pass = render_system->begin_render_pass(command_buffer, render_pass_descriptor);
        render_system->bind_render_pipeline(render_pass, pipeline);

        const glm::uvec2 window_size = window_system->get_window_size(window);
        render_system->set_viewport(render_pass, 0.0f, 0.0f, static_cast<float>(window_size.x), static_cast<float>(window_size.y), 0.0f, 1.0f);
        render_system->set_scissor(render_pass, 0, 0, window_size.x, window_size.y);

        render_system->draw(render_pass, 3, 1, 0, 0);
        render_system->end_render_pass(render_pass);

        render_system->submit_command_buffer(command_buffer);
    }

    render_system->destroy_render_pipeline(pipeline);
    render_system->destroy_pipeline_layout(pipeline_layout);
    window_system->destroy_window(window);

    return 0;
}