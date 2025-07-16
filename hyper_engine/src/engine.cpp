/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "engine.hpp"

#include <chrono>
#include <ranges>

#include <fastgltf/types.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <stb_image.h>
#include <tracy/Tracy.hpp>

#include "core/assertion.hpp"
#include "core/logger.hpp"
#include "platform/window_events.hpp"
#include "shader_interop.h"

void Engine::initialize()
{
    ZoneScoped;

    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    Logger::initialize();

    m_window_server = WindowServer::create();
    m_window = m_window_server->create_window("HyperEngine", 1280, 720);

    m_event_server = EventServer::create();
    m_input_server = InputServer::create(*m_event_server);

    const u32 width = m_window_server->get_width(m_window);
    const u32 height = m_window_server->get_height(m_window);
    m_render_server = RenderServer::create(*m_event_server, m_window_server->get_native(m_window), width, height);

    // Camera
    m_event_server->subscribe<MouseMoveEvent>(
        [this](const MouseMoveEvent &event)
        {
            m_camera.process_mouse_movement(
                event.x(), event.y(), m_input_server->is_mouse_button_pressed(MouseCode::ButtonMiddle));
        });

    m_event_server->subscribe<MouseScrollEvent>(
        [this](const MouseScrollEvent &event)
        {
            m_camera.process_mouse_scroll(event.delta_y());
        });

    m_camera_buffer = m_render_server->create_buffer({
        .label = std::nullopt,
        .size = sizeof(ShaderCamera),
        .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,
        .handle = ResourceHandle(HE_DESCRIPTOR_SET_SLOT_CAMERA),
    });

    // Scene
    m_scene_buffer = m_render_server->create_buffer({
        .label = std::nullopt,
        .size = sizeof(ShaderScene),
        .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,
        .handle = std::nullopt,
    });

    create_pbr();
    create_skybox();
    create_grid();
    create_composition();
    create_default();

    m_sponza = Asset::load("./assets/models/sponza/Sponza.gltf");
    m_damaged_helmet = Asset::load("./assets/models/DamagedHelmet.glb");

    const std::vector<GpuModel> sponza_models = upload_asset(m_sponza);
    m_renderables.insert(m_renderables.end(), sponza_models.begin(), sponza_models.end());

    const std::vector<GpuModel> damaged_helmet_models = upload_asset(m_damaged_helmet);
    m_renderables.insert(m_renderables.end(), damaged_helmet_models.begin(), damaged_helmet_models.end());

    m_event_server->subscribe<WindowCloseEvent>(
        [this](const WindowCloseEvent &)
        {
            m_running = false;
        });

    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<f64> elapsed_seconds = end_time - start_time;
    HE_INFO("Engine initialized in {:.2}s", elapsed_seconds.count());
}

void Engine::shutdown() const
{
    ZoneScoped;

    m_render_server->destroy_sampler(m_default_sampler);
    m_render_server->destroy_texture(m_default_texture);

    m_render_server->destroy_texture(m_depth_texture);
    m_render_server->destroy_sampler(m_composition_sampler);
    m_render_server->destroy_texture(m_composition_texture);
    m_render_server->destroy_render_pipeline(m_composition_pipeline);
    m_render_server->destroy_pipeline_layout(m_composition_layout);

    m_render_server->destroy_render_pipeline(m_grid_pipeline);

    m_render_server->destroy_sampler(m_irradiance_sampler);
    m_render_server->destroy_texture(m_irradiance_texture);
    m_render_server->destroy_sampler(m_skybox_sampler);
    m_render_server->destroy_texture(m_skybox_texture);
    m_render_server->destroy_render_pipeline(m_skybox_pipeline);
    m_render_server->destroy_pipeline_layout(m_skybox_layout);

    m_render_server->destroy_render_pipeline(m_pbr_pipeline);
    m_render_server->destroy_pipeline_layout(m_pbr_layout);

    m_render_server->destroy_buffer(m_scene_buffer);
    m_render_server->destroy_buffer(m_camera_buffer);

    m_window_server->destroy_window(m_window);
}

void Engine::run()
{
    f32 total_time = 0.0;
    constexpr f32 delta_time = 1.0f / 60.0f;

    f32 accumulator = 0.0;
    std::chrono::time_point current_time = std::chrono::steady_clock::now();
    while (m_running)
    {
        const std::chrono::time_point new_time = std::chrono::steady_clock::now();
        const f32 frame_time = std::chrono::duration<f32>(new_time - current_time).count();
        current_time = new_time;

        accumulator += frame_time;

        m_event_server->poll();

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

        FrameMark;
    }
}

void Engine::fixed_update(const f32 delta_time)
{
    ZoneScopedN("FixedUpdate");

    (void) delta_time;
}

void Engine::update(const f32 delta_time)
{
    ZoneScopedN("Update");

    if (m_input_server->is_key_pressed(KeyCode::W))
    {
        m_camera.process_keyboard(Camera::Movement::Forward, delta_time);
    }

    if (m_input_server->is_key_pressed(KeyCode::S))
    {
        m_camera.process_keyboard(Camera::Movement::Backward, delta_time);
    }

    if (m_input_server->is_key_pressed(KeyCode::A))
    {
        m_camera.process_keyboard(Camera::Movement::Left, delta_time);
    }

    if (m_input_server->is_key_pressed(KeyCode::D))
    {
        m_camera.process_keyboard(Camera::Movement::Right, delta_time);
    }
}

void Engine::render() const
{
    ZoneScopedN("Render");

    const CommandBufferId command_buffer = m_render_server->acquire_command_buffer();

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
        .smaller_view = glm::mat4(glm::mat3(view_matrix)),
        .near_plane = m_camera.near_plane(),
        .far_plane = m_camera.far_plane(),
        .padding_0 = 0.0,
        .padding_1 = 0.0,
    };
    m_render_server->write_buffer(command_buffer,
        {
            .buffer = m_camera_buffer,
            .offset = 0,
        },
        shader_camera);

    const ShaderScene shader_scene = {
        .irradiance_texture = m_render_server->get_texture_view_handle(m_irradiance_texture_view),
        .irradiance_sampler = m_render_server->get_sampler_handle(m_irradiance_sampler),
        .padding_0 = 0,
        .padding_1 = 0,
    };
    m_render_server->write_buffer(command_buffer,
        {
            .buffer = m_scene_buffer,
            .offset = 0,
        },
        shader_scene);

    const RenderPassId render_pass = m_render_server->begin_render_pass(
        command_buffer,
        {
            .label =
                PassLabel{
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
                    ColorAttachment{
                        .view = m_composition_texture_view,
                        .operations =
                            {
                                .load_op = LoadOperation::Clear,
                                .store_op = StoreOperation::Store,
                            },
                    },
                },
            .depth_stencil_attachment =
                DepthStencilAttachment{
                    .view = m_depth_texture_view,
                    .depth_operations =
                        {
                            .load_op = LoadOperation::Clear,
                            .store_op = StoreOperation::Store,
                        },
                },
        });

    const ResourceHandle scene_buffer_handle = m_render_server->get_buffer_handle(m_scene_buffer);
    for (const GpuModel &model : m_renderables)
    {
        glm::mat4 transform = model.transform;
        transform = glm::translate(transform, glm::vec3(0.0f, 25.0f, 0.0f));

        const ResourceHandle model_buffer_handle = m_render_server->get_buffer_handle(model.model_buffer);

        m_render_server->bind_index_buffer(render_pass, model.indices_buffer);

        for (const GpuMesh &mesh : model.meshes)
        {
            const ResourceHandle material_buffer_handle = m_render_server->get_buffer_handle(mesh.material_buffer);
            // FIXME: Pick pipeline based on material & optimization if it is the same pipeline from before
            m_render_server->bind_pipeline(render_pass, m_pbr_pipeline);

            const ObjectPushConstants mesh_push_constants = {
                .scene = scene_buffer_handle,
                .model = model_buffer_handle,
                .material = material_buffer_handle,
                .padding_0 = 0,
                .transform_matrix = transform,
            };
            m_render_server->push_constants(render_pass, mesh_push_constants);

            m_render_server->draw_indexed(
                render_pass, static_cast<u32>(mesh.index_count), 1, static_cast<u32>(mesh.start_index), 0, 0);
        }
    }

    m_render_server->end_render_pass(render_pass);

    const RenderPassId skybox_render_pass = m_render_server->begin_render_pass(
        command_buffer,
        {
            .label =
                PassLabel{
                    .name = "Skybox Render Pass",
                    .color =
                        {
                            .r = 0,
                            .g = 0,
                            .b = 255,
                        },
                },
            .color_attachments =
                {
                    ColorAttachment{
                        .view = m_composition_texture_view,
                        .operations =
                            {
                                .load_op = LoadOperation::Load,
                                .store_op = StoreOperation::Store,
                            },
                    },
                },
            .depth_stencil_attachment =
                DepthStencilAttachment{
                    .view = m_depth_texture_view,
                    .depth_operations =
                        {
                            .load_op = LoadOperation::Load,
                            .store_op = StoreOperation::None,
                        },
                },
        });
    //
    m_render_server->bind_pipeline(skybox_render_pass, m_skybox_pipeline);
    const SkyboxPushConstants skybox_push_constants = {
        .skybox_texture = m_render_server->get_texture_view_handle(m_skybox_texture_view),
        .skybox_sampler = m_render_server->get_sampler_handle(m_skybox_sampler),
        .padding_0 = 0,
        .padding_1 = 0,
    };
    m_render_server->push_constants(skybox_render_pass, skybox_push_constants);
    m_render_server->draw(skybox_render_pass, 36, 1, 0, 0);
    m_render_server->end_render_pass(skybox_render_pass);

    const RenderPassId grid_render_pass = m_render_server->begin_render_pass(
        command_buffer,
        {
            .label =
                PassLabel{
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
                    ColorAttachment{
                        .view = m_composition_texture_view,
                        .operations =
                            {
                                .load_op = LoadOperation::Load,
                                .store_op = StoreOperation::Store,
                            },
                    },
                },
            .depth_stencil_attachment =
                DepthStencilAttachment{
                    .view = m_depth_texture_view,
                    .depth_operations =
                        {
                            .load_op = LoadOperation::Load,
                            .store_op = StoreOperation::Store,
                        },
                },
        });
    m_render_server->bind_pipeline(grid_render_pass, m_grid_pipeline);
    m_render_server->draw(grid_render_pass, 6, 1, 0, 0);
    m_render_server->end_render_pass(grid_render_pass);

    const TextureViewId swapchain_texture_view = m_render_server->acquire_swapchain_texture(command_buffer);
    const RenderPassId composition_render_pass = m_render_server->begin_render_pass(
        command_buffer,
        {
            .label =
                PassLabel{
                    .name = "Composition Render Pass",
                    .color =
                        {
                            .r = 255,
                            .g = 255,
                            .b = 0,
                        },
                },
            .color_attachments =
                {
                    ColorAttachment{
                        .view = swapchain_texture_view,
                        .operations =
                            {
                                .load_op = LoadOperation::Clear,
                                .store_op = StoreOperation::Store,
                            },
                    },
                },
            .depth_stencil_attachment = std::nullopt,
        });
    m_render_server->bind_pipeline(composition_render_pass, m_composition_pipeline);
    const CompositionPushConstants composition_push_constants = {
        .composition_texture = m_render_server->get_texture_view_handle(m_composition_texture_view),
        .composition_sampler = m_render_server->get_sampler_handle(m_composition_sampler),
        .padding_0 = 0,
        .padding_1 = 0,
    };
    m_render_server->push_constants(composition_render_pass, composition_push_constants);
    m_render_server->draw(composition_render_pass, 3, 1, 0, 0);
    m_render_server->end_render_pass(composition_render_pass);

    m_render_server->submit_command_buffer(command_buffer);
}

void Engine::create_pbr()
{
    m_pbr_layout = m_render_server->create_pipeline_layout({
        .label = std::nullopt,
        .push_constant_size = sizeof(ObjectPushConstants),
    });

    const ShaderId vertex_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/pbr_shader.hlsl",
    });

    const ShaderId fragment_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/pbr_shader.hlsl",
    });

    m_pbr_pipeline = m_render_server->create_render_pipeline({
        .label = std::nullopt,
        .layout = m_pbr_layout,
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .primitive_state =
            {
                .topology = PrimitiveTopology::TriangleList,
                .front_face = FrontFace::CounterClockwise,
                .cull_mode = Face::Back,
                .polygon_mode = PolygonMode::Fill,
            },
        .color_attachment_states =
            {
                ColorAttachmentState{
                    .format = Format::Rgba16Sfloat,
                    .blend_state =
                        {
                            .enable = false,
                            .src_factor = BlendFactor::One,
                            .dst_factor = BlendFactor::Zero,
                            .operation = BlendOperation::Add,
                            .alpha_src_factor = BlendFactor::One,
                            .alpha_dst_factor = BlendFactor::Zero,
                            .alpha_operation = BlendOperation::Add,
                            .color_writes = ColorWrites::All,
                        },
                },
            },
        .depth_stencil_state =
            DepthStencilState{
                .depth_test_enable = true,
                .depth_write_enable = true,
                .depth_format = Format::D32Sfloat,
                .depth_compare_operation = CompareOperation::Less,
                .depth_bias_state = {
                        .enable = false,
                        .constant = 0.0f,
                        .clamp = 0.0f,
                        .slope = 0.0f,
                    },
            },
    });

    m_render_server->destroy_shader(fragment_shader);
    m_render_server->destroy_shader(vertex_shader);
}

void Engine::create_skybox()
{
    const PipelineLayoutId equirectangular_layout = m_render_server->create_pipeline_layout({
        .label = std::nullopt,
        .push_constant_size = sizeof(EquirectangularPushConstants),
    });

    const ShaderId equirectangular_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Compute,
        .entry = "main",
        .path = "./assets/shaders/equirectangular_to_cubemap.hlsl",
    });

    const ComputePipelineId equirectangular_pipeline = m_render_server->create_compute_pipeline({
        .label = std::nullopt,
        .layout = equirectangular_layout,
        .shader = equirectangular_shader,
    });

    m_render_server->destroy_shader(equirectangular_shader);

    i32 width { 0 };
    i32 height { 0 };
    i32 channels { 0 };
    f32 *data = stbi_loadf("./assets/images/mirrored_hall_4k.hdr", &width, &height, &channels, 0);
    HE_ASSERT(data != nullptr);

    std::vector<f32> new_data;
    for (u32 i { 0 }; i != static_cast<u32>(width * height * 3); i += 3)
    {
        new_data.push_back(data[i + 0]);
        new_data.push_back(data[i + 1]);
        new_data.push_back(data[i + 2]);
        new_data.push_back(1.0f);
    }

    stbi_image_free(data);

    const TextureId equirectangular_texture = m_render_server->create_texture({
        .label = std::nullopt,
        .extent = {
            .width = static_cast<u32>(width),
            .height = static_cast<u32>(height),
            .depth = 1,
        },
        .mip_levels = 1,
        .format = Format::Rgba32Sfloat,
        .dimension = Dimension::D2,
        .usage = TextureUsage::TransferDst | TextureUsage::Resource,
    });

    const TextureViewId equirectangular_texture_view = m_render_server->create_texture_view({
        .label = std::nullopt,
        .texture = equirectangular_texture,
        .dimension = ViewDimension::D2,
        .base_mip_level = 0,
        .mip_levels = 1,
        .base_array_layer = 0,
        .array_layers = 1,
    });

    {
        const CommandBufferId command_buffer = m_render_server->acquire_command_buffer();
        m_render_server->write_texture(
            command_buffer,
            {
                .texture = equirectangular_texture,
                .offset =
                    {
                        .x = 0,
                        .y = 0,
                        .z = 0,
                    },
                .mip_level = 0,
                .array_index = 0,
            },
            new_data.data(),
            width * height * 4 * sizeof(f32),
    {
                .width = static_cast<u32>(width),
                .height = static_cast<u32>(height),
                .depth = 1,
            });

        m_render_server->submit_command_buffer(command_buffer);
    }

    m_render_server->wait_idle();

    m_skybox_layout = m_render_server->create_pipeline_layout({
        .label = std::nullopt,
        .push_constant_size = sizeof(SkyboxPushConstants),
    });

    const ShaderId skybox_vertex_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/skybox_shader.hlsl",
    });

    const ShaderId skybox_fragment_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/skybox_shader.hlsl",
    });

    m_skybox_pipeline = m_render_server->create_render_pipeline({
        .label = std::nullopt,
        .layout = m_skybox_layout,
        .vertex_shader = skybox_vertex_shader,
        .fragment_shader = skybox_fragment_shader,
        .primitive_state =
            {
                .topology = PrimitiveTopology::TriangleList,
                .front_face = FrontFace::CounterClockwise,
                .cull_mode = Face::None,
                .polygon_mode = PolygonMode::Fill,
            },
        .color_attachment_states =
            {
               ColorAttachmentState {
                    .format = Format::Rgba16Sfloat,
                    .blend_state =
                        {
                            .enable = false,
                            .src_factor = BlendFactor::One,
                            .dst_factor = BlendFactor::Zero,
                            .operation = BlendOperation::Add,
                            .alpha_src_factor = BlendFactor::One,
                            .alpha_dst_factor = BlendFactor::Zero,
                            .alpha_operation = BlendOperation::Add,
                            .color_writes = ColorWrites::All,
                        },
                    },
            },
        .depth_stencil_state =
            DepthStencilState {
                .depth_test_enable = true,
                .depth_write_enable = false,
                .depth_format = Format::D32Sfloat,
                .depth_compare_operation = CompareOperation::LessEqual,
                .depth_bias_state =
                    {
                        .enable = false,
                        .constant = 0.0f,
                        .clamp = 0.0f,
                        .slope = 0.0f,
                    },
            },
    });

    m_render_server->destroy_shader(skybox_fragment_shader);
    m_render_server->destroy_shader(skybox_vertex_shader);

    m_skybox_texture = m_render_server->create_texture({
        .label = std::nullopt,
        .extent = {
            .width = static_cast<u32>(height),
            .height = static_cast<u32>(height),
            .depth = 6,
        },
        .mip_levels = 1,
        .format = Format::Rgba16Sfloat,
        .dimension = Dimension::D2,
        .usage = TextureUsage::TransferDst | TextureUsage::Storage | TextureUsage::Resource,
    });

    m_skybox_texture_view = m_render_server->create_texture_view({
        .label = std::nullopt,
        .texture = m_skybox_texture,
        .dimension = ViewDimension::Cube,
        .base_mip_level = 0,
        .mip_levels = 1,
        .base_array_layer = 0,
        .array_layers = 6,
    });

    m_skybox_sampler = m_render_server->create_sampler({
        .label = std::nullopt,
        .mag_filter = Filter::Linear,
        .min_filter = Filter::Linear,
        .mipmap_filter = Filter::Linear,
        .address_mode_u = AddressMode::ClampToEdge,
        .address_mode_v = AddressMode::ClampToEdge,
        .address_mode_w = AddressMode::ClampToEdge,
        .compare_operation = CompareOperation::Less,
        .min_lod = 0.0f,
        .max_lod = 1.0f,
        .border_color = BorderColor::TransparentBlack,
    });

    {
        const u32 workgroups = (height + 15) / 16;

        const CommandBufferId command_buffer = m_render_server->acquire_command_buffer();
        const ComputePassId compute_pass = m_render_server->begin_compute_pass(command_buffer,
            {
                .label = PassLabel {
                    .name = "Equirectangular to Cubemap",
                    .color = {
                        .r = 255,
                        .g = 0,
                        .b = 255,
                    },
                },
            });
        m_render_server->bind_pipeline(compute_pass, equirectangular_pipeline);
        const EquirectangularPushConstants equirectangular_push_constants = {
            .equirectangular_texture = m_render_server->get_texture_view_handle(equirectangular_texture_view),
            .equirectangular_width = static_cast<u32>(width),
            .equirectangular_height = static_cast<u32>(height),
            .padding_0 = 0,
            .skybox_texture = m_render_server->get_texture_view_handle(m_skybox_texture_view),
            .skybox_size = static_cast<u32>(height),
            .padding_1 = 0,
        };
        m_render_server->push_constants(compute_pass, equirectangular_push_constants);
        m_render_server->dispatch(compute_pass, workgroups, workgroups, 6);
        m_render_server->end_compute_pass(compute_pass);

        m_render_server->submit_command_buffer(command_buffer);
    }

    m_render_server->wait_idle();

    m_render_server->destroy_pipeline_layout(equirectangular_layout);
    m_render_server->destroy_compute_pipeline(equirectangular_pipeline);
    m_render_server->destroy_texture(equirectangular_texture);

    const PipelineLayoutId irradiance_layout = m_render_server->create_pipeline_layout({
        .label = std::nullopt,
        .push_constant_size = sizeof(IrradiancePushConstants),
    });

    const ShaderId irradiance_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Compute,
        .entry = "main",
        .path = "./assets/shaders/irradiance.hlsl",
    });

    const ComputePipelineId irradiance_pipeline = m_render_server->create_compute_pipeline({
        .label = std::nullopt,
        .layout = irradiance_layout,
        .shader = irradiance_shader,
    });

    m_render_server->destroy_shader(irradiance_shader);

    m_irradiance_texture = m_render_server->create_texture({
        .label = std::nullopt,
        .extent = {
            .width = static_cast<u32>(height),
            .height = static_cast<u32>(height),
            .depth = 6,
        },
        .mip_levels = 1,
        .format = Format::Rgba16Sfloat,
        .dimension = Dimension::D2,
        .usage = TextureUsage::TransferDst | TextureUsage::Storage | TextureUsage::Resource,
    });

    m_irradiance_texture_view = m_render_server->create_texture_view({
        .label = std::nullopt,
        .texture = m_irradiance_texture,
        .dimension = ViewDimension::Cube,
        .base_mip_level = 0,
        .mip_levels = 1,
        .base_array_layer = 0,
        .array_layers = 6,
    });

    m_irradiance_sampler = m_render_server->create_sampler({
        .label = std::nullopt,
        .mag_filter = Filter::Linear,
        .min_filter = Filter::Linear,
        .mipmap_filter = Filter::Linear,
        .address_mode_u = AddressMode::ClampToEdge,
        .address_mode_v = AddressMode::ClampToEdge,
        .address_mode_w = AddressMode::ClampToEdge,
        .compare_operation = CompareOperation::Less,
        .min_lod = 0.0f,
        .max_lod = 1.0f,
        .border_color = BorderColor::TransparentBlack,
    });

    {
        const u32 workgroups = (height + 15) / 16;

        const CommandBufferId command_buffer = m_render_server->acquire_command_buffer();
        const ComputePassId compute_pass = m_render_server->begin_compute_pass(command_buffer,
            {
                .label = PassLabel {
                    .name = "Generate Irradiance map",
                    .color = {
                        .r = 255,
                        .g = 255,
                        .b = 0,
                    },
                },
            });
        m_render_server->bind_pipeline(compute_pass, irradiance_pipeline);
        const IrradiancePushConstants irradiance_push_constants = {
            .skybox_texture = m_render_server->get_texture_view_handle(m_skybox_texture_view),
            .skybox_sampler = m_render_server->get_sampler_handle(m_skybox_sampler),
            .irradiance_texture = m_render_server->get_texture_view_handle(m_irradiance_texture_view),
            .size = static_cast<u32>(height),
        };
        m_render_server->push_constants(compute_pass, irradiance_push_constants);
        m_render_server->dispatch(compute_pass, workgroups, workgroups, 6);
        m_render_server->end_compute_pass(compute_pass);

        m_render_server->submit_command_buffer(command_buffer);
    }

    m_render_server->wait_idle();

    m_render_server->destroy_pipeline_layout(irradiance_layout);
    m_render_server->destroy_compute_pipeline(irradiance_pipeline);
}

void Engine::create_grid()
{
    const ShaderId grid_vertex_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/grid_shader.hlsl",
    });

    const ShaderId grid_fragment_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/grid_shader.hlsl",
    });

    m_grid_pipeline = m_render_server->create_render_pipeline({
        .label = std::nullopt,
        .layout = m_pbr_layout,
        .vertex_shader = grid_vertex_shader,
        .fragment_shader = grid_fragment_shader,
        .primitive_state =
            {
                .topology = PrimitiveTopology::TriangleList,
                .front_face = FrontFace::CounterClockwise,
                .cull_mode = Face::None,
                .polygon_mode = PolygonMode::Fill,
            },
        .color_attachment_states =
            {
                ColorAttachmentState{
                    .format = Format::Rgba16Sfloat,
                    .blend_state =
                        {
                            .enable = true,
                            .src_factor = BlendFactor::SrcAlpha,
                            .dst_factor = BlendFactor::One,
                            .operation = BlendOperation::Add,
                            .alpha_src_factor = BlendFactor::One,
                            .alpha_dst_factor = BlendFactor::Zero,
                            .alpha_operation = BlendOperation::Add,
                            .color_writes = ColorWrites::All,
                        },
                },
            },
        .depth_stencil_state =
            DepthStencilState{
                .depth_test_enable = true,
                .depth_write_enable = true,
                .depth_format = Format::D32Sfloat,
                .depth_compare_operation = CompareOperation::Less,
                .depth_bias_state =
                    {
                        .enable = false,
                        .constant = 0.0f,
                        .clamp = 0.0f,
                        .slope = 0.0f,
                    },
            },
    });

    m_render_server->destroy_shader(grid_fragment_shader);
    m_render_server->destroy_shader(grid_vertex_shader);
}

void Engine::create_composition()
{
    m_composition_layout = m_render_server->create_pipeline_layout({
        .label = std::nullopt,
        .push_constant_size = sizeof(CompositionPushConstants),
    });

    const ShaderId composition_vertex_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/composition_shader.hlsl",
    });

    const ShaderId composition_fragment_shader = m_render_server->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/composition_shader.hlsl",
    });

    m_composition_pipeline = m_render_server->create_render_pipeline({
        .label = std::nullopt,
        .layout = m_composition_layout,
        .vertex_shader = composition_vertex_shader,
        .fragment_shader = composition_fragment_shader,
        .primitive_state =
            {
                .topology = PrimitiveTopology::TriangleList,
                .front_face = FrontFace::CounterClockwise,
                .cull_mode = Face::Back,
                .polygon_mode = PolygonMode::Fill,
            },
        .color_attachment_states =
            {
                ColorAttachmentState{
                    .format = Format::Bgra8Unorm,
                    .blend_state =
                        {
                            .enable = false,
                            .src_factor = BlendFactor::One,
                            .dst_factor = BlendFactor::Zero,
                            .operation = BlendOperation::Add,
                            .alpha_src_factor = BlendFactor::One,
                            .alpha_dst_factor = BlendFactor::Zero,
                            .alpha_operation = BlendOperation::Add,
                            .color_writes = ColorWrites::All,
                        },
                },
            },
        .depth_stencil_state =
            DepthStencilState{
                .depth_test_enable = false,
                .depth_write_enable = false,
                .depth_format = Format::D32Sfloat,
                .depth_compare_operation = CompareOperation::Less,
                .depth_bias_state =
                    {
                        .enable = false,
                        .constant = 0.0f,
                        .clamp = 0.0f,
                        .slope = 0.0f,
                    },
            },
    });

    m_render_server->destroy_shader(composition_fragment_shader);
    m_render_server->destroy_shader(composition_vertex_shader);

    const u32 width = m_window_server->get_width(m_window);
    const u32 height = m_window_server->get_height(m_window);
    m_composition_texture = m_render_server->create_texture({
        .label = std::nullopt,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
        .mip_levels = 1,
        .format = Format::Rgba16Sfloat,
        .dimension = Dimension::D2,
        .usage = TextureUsage::RenderAttachment | TextureUsage::Resource,
    });

    m_composition_texture_view = m_render_server->create_texture_view({
        .label = std::nullopt,
        .texture = m_composition_texture,
        .dimension = ViewDimension::D2,
        .base_mip_level = 0,
        .mip_levels = 1,
        .base_array_layer = 0,
        .array_layers = 1,
    });

    m_composition_sampler = m_render_server->create_sampler({
        .label = std::nullopt,
        .mag_filter = Filter::Nearest,
        .min_filter = Filter::Nearest,
        .mipmap_filter = Filter::Nearest,
        .address_mode_u = AddressMode::Repeat,
        .address_mode_v = AddressMode::Repeat,
        .address_mode_w = AddressMode::Repeat,
        .compare_operation = CompareOperation::Never,
        .min_lod = 0.0f,
        .max_lod = 1.0f,
        .border_color = BorderColor::TransparentBlack,
    });

    m_depth_texture = m_render_server->create_texture({
        .label = std::nullopt,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
        .mip_levels = 1,
        .format = Format::D32Sfloat,
        .dimension = Dimension::D2,
        .usage = TextureUsage::RenderAttachment,
    });

    m_depth_texture_view = m_render_server->create_texture_view({
        .label = std::nullopt,
        .texture = m_depth_texture,
        .dimension = ViewDimension::D2,
        .base_mip_level = 0,
        .mip_levels = 1,
        .base_array_layer = 0,
        .array_layers = 1,
    });

    m_event_server->subscribe<WindowResizeEvent>(
        [this](const WindowResizeEvent &event)
        {
            m_render_server->destroy_texture(m_depth_texture);
            m_render_server->destroy_texture(m_composition_texture);

            m_composition_texture = m_render_server->create_texture({
                .label = std::nullopt,
                .extent = {
                    .width = event.width(),
                    .height = event.height(),
                    .depth = 1,
                },
                .mip_levels = 1,
                .format = Format::Rgba16Sfloat,
                .dimension = Dimension::D2,
                .usage = TextureUsage::RenderAttachment | TextureUsage::Resource,
            });

            m_composition_texture_view = m_render_server->create_texture_view({
                .label = std::nullopt,
                .texture = m_composition_texture,
                .dimension = ViewDimension::D2,
                .base_mip_level = 0,
                .mip_levels = 1,
                .base_array_layer = 0,
                .array_layers = 1,
            });

            m_depth_texture = m_render_server->create_texture({
                .label = std::nullopt,
                .extent = {
                    .width = event.width(),
                    .height = event.height(),
                    .depth = 1,
                },
                .mip_levels = 1,
                .format = Format::D32Sfloat,
                .dimension = Dimension::D2,
                .usage = TextureUsage::RenderAttachment,
            });

            m_depth_texture_view = m_render_server->create_texture_view({
                .label = std::nullopt,
                .texture = m_depth_texture,
                .dimension = ViewDimension::D2,
                .base_mip_level = 0,
                .mip_levels = 1,
                .base_array_layer = 0,
                .array_layers = 1,
            });
        });
}

void Engine::create_default()
{
    m_default_texture = m_render_server->create_texture({
        .label = std::nullopt,
        .extent = {
            .width = 16,
            .height = 16,
            .depth = 1,
        },
        .mip_levels = 1,
        .format = Format::Rgba8Unorm,
        .dimension = Dimension::D2,
        .usage = TextureUsage::TransferDst | TextureUsage::Resource,
    });

    m_default_texture_view = m_render_server->create_texture_view({
        .label = std::nullopt,
        .texture = m_default_texture,
        .dimension = ViewDimension::D2,
        .base_mip_level = 0,
        .mip_levels = 1,
        .base_array_layer = 0,
        .array_layers = 1,
    });

    m_default_sampler = m_render_server->create_sampler({
        .label = std::nullopt,
        .mag_filter = Filter::Nearest,
        .min_filter = Filter::Nearest,
        .mipmap_filter = Filter::Nearest,
        .address_mode_u = AddressMode::Repeat,
        .address_mode_v = AddressMode::Repeat,
        .address_mode_w = AddressMode::Repeat,
        .compare_operation = CompareOperation::Never,
        .min_lod = 0.0f,
        .max_lod = 1.0f,
        .border_color = BorderColor::TransparentBlack,
    });

    const CommandBufferId command_buffer = m_render_server->acquire_command_buffer();

    const u32 black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
    const u32 magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));

    std::array<u32, 16 * 16> pixels = {};
    for (usize x = 0; x < 16; x++)
    {
        for (usize y = 0; y < 16; y++)
        {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }

    m_render_server->write_texture(
        command_buffer,
        {
            .texture = m_default_texture,
            .offset =
                {
                    .x = 0,
                    .y = 0,
                    .z = 0,
                },
            .mip_level = 0,
            .array_index = 0,
        },
        pixels.data(),
        pixels.size() * sizeof(u32),
        {
            .width = 16,
            .height = 16,
            .depth = 1,
        });

    m_render_server->submit_command_buffer(command_buffer);
}

std::vector<Engine::GpuModel> Engine::upload_asset(const Asset &asset)
{
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    std::vector<GpuModel> gpu_models;

    const CommandBufferId command_buffer = m_render_server->acquire_command_buffer();
    for (const Asset::Scene &scene : asset.scenes())
    {
        for (const usize node_index : scene.node_indices)
        {
            const Asset::Node *node = asset.nodes()[node_index].get();
            upload_model(command_buffer, asset, node, glm::mat4(1.0f), gpu_models);
        }
    }
    m_render_server->submit_command_buffer(command_buffer);

    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<f64> elapsed_seconds = end_time - start_time;
    HE_INFO("Asset uploaded in {:.2}s", elapsed_seconds.count());

    return gpu_models;
}

void Engine::upload_model(const CommandBufferId command_buffer,
    const Asset &asset,
    const Asset::Node *node,
    const glm::mat4 &parent_transform,
    std::vector<GpuModel> &models)
{
    const glm::mat4 transform = parent_transform * node->local_transform;

    if (node->model_index.has_value())
    {
        const usize model_index = node->model_index.value();
        const Asset::Model &asset_model = asset.models()[model_index];

        const BufferId positions_buffer = m_render_server->create_buffer({
            .label = std::nullopt,
            .size = asset_model.positions.size() * sizeof(glm::vec3),
            .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,
        });
        m_render_server->write_buffer(command_buffer,
            {
                .buffer = positions_buffer,
                .offset = 0,
            },
            asset_model.positions.data(), asset_model.positions.size() * sizeof(glm::vec3));

        const BufferId normals_buffer = m_render_server->create_buffer({
            .label = std::nullopt,
            .size = asset_model.normals.size() * sizeof(glm::vec3),
            .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,
        });
        m_render_server->write_buffer(command_buffer,
            {
                .buffer = normals_buffer,
                .offset = 0,
            },
            asset_model.normals.data(), asset_model.normals.size() * sizeof(glm::vec3));

        const BufferId tangents_buffer = m_render_server->create_buffer({
            .label = std::nullopt,
            .size = asset_model.tangents.size() * sizeof(glm::vec4),
            .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,
        });
        m_render_server->write_buffer(command_buffer,
            {
                .buffer = tangents_buffer,
                .offset = 0,
            },
            asset_model.tangents.data(), asset_model.tangents.size() * sizeof(glm::vec4));

        const BufferId colors_buffer = m_render_server->create_buffer({
            .label = std::nullopt,
            .size = asset_model.colors.size() * sizeof(glm::vec3),
            .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,

        });
        m_render_server->write_buffer(command_buffer,
            {
                .buffer = colors_buffer,
                .offset = 0,
            },
            asset_model.colors.data(), asset_model.colors.size() * sizeof(glm::vec3));

        const BufferId uvs_buffer = m_render_server->create_buffer({
            .label = std::nullopt,
            .size = asset_model.uvs.size() * sizeof(glm::vec2),
            .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,

        });
        m_render_server->write_buffer(command_buffer,
            {
                .buffer = uvs_buffer,
                .offset = 0,
            },
            asset_model.uvs.data(), asset_model.uvs.size() * sizeof(glm::vec2));

        const ShaderModel shader_model = {
            .positions = m_render_server->get_buffer_handle(positions_buffer),
            .normals = m_render_server->get_buffer_handle(normals_buffer),
            .tangents = m_render_server->get_buffer_handle(tangents_buffer),
            .colors = m_render_server->get_buffer_handle(colors_buffer),
            .uvs = m_render_server->get_buffer_handle(uvs_buffer),
        };

        const BufferId model_buffer = m_render_server->create_buffer({
            .label = std::nullopt,
            .size = sizeof(ShaderModel),
            .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,
        });
        m_render_server->write_buffer(command_buffer,
            {
                .buffer = model_buffer,
                .offset = 0,
            },
            shader_model);

        const BufferId indices_buffer = m_render_server->create_buffer({
            .label = std::nullopt,
            .size = asset_model.indices.size() * sizeof(u32),
            .usage = BufferUsage::TransferDst | BufferUsage::Index,
        });
        m_render_server->write_buffer(command_buffer,
            {
                .buffer = indices_buffer,
                .offset = 0,
            },
            asset_model.indices.data(), asset_model.indices.size() * sizeof(u32));

        std::vector<GpuMesh> meshes;
        for (const Asset::Mesh &asset_mesh : asset_model.meshes)
        {
            const Asset::Material &asset_material = asset.materials()[asset_mesh.material_index];

            const auto create_texture = [&](const std::optional<usize> texture_index, const Format format)
            {
                if (!texture_index.has_value())
                {
                    return m_render_server->get_texture_view_handle(m_default_texture_view);
                }

                const Asset::Texture &asset_texture = asset.textures()[texture_index.value()];

                const TextureId texture = m_render_server->create_texture({
                        .label = std::nullopt,
                        .extent = {
                            .width = asset_texture.width,
                            .height = asset_texture.height,
                            .depth = 1,
                        },
                        .mip_levels = 1,
                        .format = format,
                        .dimension = Dimension::D2,
                        .usage = TextureUsage::TransferDst | TextureUsage::Resource,
                    });

                m_render_server->write_texture(
                    command_buffer,
                    {
                        .texture = texture,
                        .offset =
                            {
                                .x = 0,
                                .y = 0,
                                .z = 0,
                            },
                        .mip_level = 0,
                        .array_index = 0,
                    },
                    asset_texture.data.data(),
                    static_cast<u32>(asset_texture.width) * static_cast<u32>(asset_texture.height) *
                    asset_texture.channels,
                {
                        .width = static_cast<u32>(asset_texture.width),
                        .height = static_cast<u32>(asset_texture.height),
                        .depth = 1,
                    });

                // FIXME
                const TextureViewId texture_view = m_render_server->create_texture_view({
                    .label = std::nullopt,
                    .texture = texture,
                    .dimension = ViewDimension::D2,
                    .base_mip_level = 0,
                    .mip_levels = 1,
                    .base_array_layer = 0,
                    .array_layers = 1,
                });

                return m_render_server->get_texture_view_handle(texture_view);
            };

            const auto create_sampler = [&](const std::optional<usize> sampler_index)
            {
                if (!sampler_index.has_value())
                {
                    return m_render_server->get_sampler_handle(m_default_sampler);
                }
                const Asset::Sampler &asset_sampler = asset.samplers()[asset_material.base_color_sampler_index.value()];

                const SamplerId sampler = m_render_server->create_sampler({
                    .label = std::nullopt,
                    .mag_filter = asset_sampler.mag_filter,
                    .min_filter = asset_sampler.min_filter,
                    .mipmap_filter = asset_sampler.min_filter,
                    .address_mode_u = AddressMode::Repeat,
                    .address_mode_v = AddressMode::Repeat,
                    .address_mode_w = AddressMode::Repeat,
                    .compare_operation = CompareOperation::Never,
                    .min_lod = 0.0f,
                    .max_lod = 1.0f,
                    .border_color = BorderColor::TransparentBlack,
                });

                return m_render_server->get_sampler_handle(sampler);
            };

            const ResourceHandle color_texture = create_texture(asset_material.base_color_texture_index, Format::Rgba8Srgb);
            const ResourceHandle color_sampler = create_sampler(asset_material.base_color_sampler_index);

            const ResourceHandle metal_roughness_texture
                = create_texture(asset_material.metallic_roughness_texture_index, Format::Rgba8Unorm);
            const ResourceHandle metal_roughness_sampler = create_sampler(asset_material.metallic_roughness_sampler_index);

            const ResourceHandle normal_texture = create_texture(asset_material.normal_texture_index, Format::Rgba8Unorm);
            const ResourceHandle normal_sampler = create_sampler(asset_material.normal_sampler_index);

            const f32 normal_scale = asset_material.normal_scale;

            const ShaderMaterial shader_material = {
                .albedo_factors = asset_material.color_factors,
                .albedo_texture = color_texture,
                .albedo_sampler = color_sampler,
                .padding_0 = 0,
                .padding_1 = 0,
                .metal_roughness_factors = asset_material.metallic_roughness_factor,
                .metal_roughness_texture = metal_roughness_texture,
                .metal_roughness_sampler = metal_roughness_sampler,
                .normal_texture = normal_texture,
                .normal_sampler = normal_sampler,
                .normal_scale = normal_scale,
                .padding_2 = 0,
            };

            const BufferId material_buffer = m_render_server->create_buffer({
                .label = std::nullopt,
                .size = sizeof(ShaderMaterial),
                .usage = BufferUsage::TransferDst | BufferUsage::Storage | BufferUsage::Resource,
            });
            m_render_server->write_buffer(command_buffer,
                {
                    .buffer = material_buffer,
                    .offset = 0,
                },
                shader_material);

            const GpuMesh mesh = {
                .start_index = asset_mesh.start_index,
                .index_count = asset_mesh.index_count,
                .material_buffer = material_buffer,
            };

            meshes.push_back(mesh);
        }

        models.push_back({
            .transform = transform,
            .model_buffer = model_buffer,
            .indices_buffer = indices_buffer,
            .meshes = meshes,
        });
    }

    for (const Asset::Node *child : node->children)
    {
        upload_model(command_buffer, asset, child, transform, models);
    }
}
