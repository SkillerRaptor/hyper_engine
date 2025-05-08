/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "engine.hpp"

#include <chrono>

#include <fastgltf/types.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <tracy/Tracy.hpp>

#include "core/logger.hpp"
#include "shader_interop.h"
#include "systems/window/window_events.hpp"

void Engine::initialize()
{
    ZoneScoped;

    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    Logger::initialize();

    m_window_system = new WindowSystem();
    HE_ASSERT(m_window_system != nullptr);
    m_window_system->initialize();

    m_window = m_window_system->create_window({
        .title = "HyperEngine",
        .width = 1280,
        .height = 720,
    });

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

    m_camera_buffer = m_render_system->create_buffer({
        .label = std::nullopt,
        .size = sizeof(ShaderCamera),
        .usage =
            {
                BufferUsage::Storage,
                BufferUsage::ShaderResource,
            },
        .handle = ResourceHandle(HE_DESCRIPTOR_SET_SLOT_CAMERA),
    });

    // Scene
    m_scene_buffer = m_render_system->create_buffer({
        .label = std::nullopt,
        .size = sizeof(ShaderScene),
        .usage =
            {
                BufferUsage::Storage,
                BufferUsage::ShaderResource,
            },
    });

    // Default
    m_default_texture = m_render_system->create_texture({
        .label = std::nullopt,
        .width = 16,
        .height = 16,
        .depth = 1,
        .array_size = 1,
        .mip_levels = 1,
        .format = Format::Rgba8Unorm,
        .dimension = Dimension::Texture2D,
        .usage = TextureUsage::ShaderResource,
    });

    const CommandBufferId command_buffer = m_render_system->acquire_command_buffer();

    const uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
    const uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));

    std::array<uint32_t, 16 * 16> pixels = {};
    for (size_t x = 0; x < 16; x++)
    {
        for (size_t y = 0; y < 16; y++)
        {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }
    m_render_system->write_texture(
        command_buffer,
        {
            .texture = m_default_texture,
            .offset =
                {
                    .x = 0,
                    .y = 0,
                    .z = 0,
                },
            .extent =
                {
                    .width = 16,
                    .height = 16,
                    .depth = 1,
                },
            .mip_level = 0,
            .array_index = 0,
        },
        pixels.data(),
        pixels.size() * sizeof(uint32_t));

    m_render_system->submit_command_buffer(command_buffer);

    m_default_sampler = m_render_system->create_sampler({
        .label = std::nullopt,
        .mag_filter = Filter::Nearest,
        .min_filter = Filter::Nearest,
        .mipmap_filter = Filter::Nearest,
        .address_mode_u = AddressMode::Repeat,
        .address_mode_v = AddressMode::Repeat,
        .address_mode_w = AddressMode::Repeat,
        .mip_lod_bias = 0.0f,
        .compare_operation = CompareOperation::Never,
        .min_lod = 0.0f,
        .max_lod = 1.0f,
        .border_color = BorderColor::TransparentBlack,
    });

    // Rendering
    m_pipeline_layout = m_render_system->create_pipeline_layout({
        .label = std::nullopt,
        .push_constant_size = sizeof(ObjectPushConstants),
    });

    const ShaderId vertex_shader = m_render_system->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/mesh_shader.hlsl",
    });

    const ShaderId fragment_shader = m_render_system->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/mesh_shader.hlsl",
    });

    m_render_pipeline = m_render_system->create_render_pipeline({
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
                .cull_mode = Face::Back,
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
    });

    const ShaderId grid_vertex_shader = m_render_system->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Vertex,
        .entry = "vs_main",
        .path = "./assets/shaders/grid_shader.hlsl",
    });

    const ShaderId grid_fragment_shader = m_render_system->create_shader({
        .label = std::nullopt,
        .type = ShaderType::Fragment,
        .entry = "fs_main",
        .path = "./assets/shaders/grid_shader.hlsl",
    });

    m_grid_pipeline = m_render_system->create_render_pipeline({
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
    });

    m_render_system->destroy_shader(grid_fragment_shader);
    m_render_system->destroy_shader(grid_vertex_shader);

    const glm::uvec2 window_size = m_window_system->get_window_size(m_window);
    m_depth_texture = m_render_system->create_texture({
        .label = std::nullopt,
        .width = window_size.x,
        .height = window_size.y,
        .depth = 1,
        .array_size = 1,
        .mip_levels = 1,
        .format = Format::D32Sfloat,
        .dimension = Dimension::Texture2D,
        .usage = TextureUsage::RenderAttachment,
    });

    m_window_system->register_listener<WindowResizeEvent>(
        [this](const WindowResizeEvent &event)
        {
            m_render_system->destroy_texture(m_depth_texture);

            m_depth_texture = m_render_system->create_texture({
                .label = std::nullopt,
                .width = event.width(),
                .height = event.height(),
                .depth = 1,
                .array_size = 1,
                .mip_levels = 1,
                .format = Format::D32Sfloat,
                .dimension = Dimension::Texture2D,
                .usage = TextureUsage::RenderAttachment,
            });
        });

    m_render_system->destroy_shader(fragment_shader);
    m_render_system->destroy_shader(vertex_shader);

    m_sponza = Asset::load("./assets/models/sponza/Sponza.gltf");
    m_renderables = upload_asset(m_sponza);

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
    ZoneScoped;

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

        FrameMark;
    }
}

void Engine::fixed_update(const float delta_time)
{
    ZoneScoped;

    (void) delta_time;
}

void Engine::update(const float delta_time)
{
    ZoneScoped;

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
    ZoneScoped;

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
    m_render_system->write_buffer(
        command_buffer,
        {
            .buffer = m_camera_buffer,
            .offset = 0,
        },
        shader_camera);

    constexpr ShaderScene shader_scene = {
        .ambient_color = glm::vec4(0.1f),
        .sunlight_direction = glm::vec4(0.0f, 1.0f, 0.5f, 1.0f),
        .sunlight_color = glm::vec4(1.0f),
        .padding_0 = glm::vec4(0.0f),
    };
    m_render_system->write_buffer(
        command_buffer,
        {
            .buffer = m_scene_buffer,
            .offset = 0,
        },
        shader_scene);

    const TextureId swapchain_texture = m_render_system->acquire_swapchain_texture(command_buffer);

    const RenderPassId render_pass = m_render_system->begin_render_pass(
        command_buffer,
        {
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
        });

    const ResourceHandle scene_buffer_handle = m_render_system->get_buffer_handle(m_scene_buffer);
    for (const GpuModel &model : m_renderables)
    {
        glm::mat4 transform = model.transform;
        transform = glm::translate(transform, glm::vec3(0.0f, 10.0f, 0.0f));

        const ResourceHandle model_buffer_handle = m_render_system->get_buffer_handle(model.model_buffer);

        m_render_system->bind_index_buffer(render_pass, model.indices_buffer);

        for (const GpuMesh &mesh : model.meshes)
        {
            const ResourceHandle material_buffer_handle = m_render_system->get_buffer_handle(mesh.material_buffer);
            // FIXME: Pick pipeline based on material & optimization if it is the same pipeline from before
            m_render_system->bind_pipeline(render_pass, m_render_pipeline);

            const ObjectPushConstants mesh_push_constants = {
                .scene = scene_buffer_handle,
                .model = model_buffer_handle,
                .material = material_buffer_handle,
                .padding_0 = 0,
                .transform_matrix = transform,
            };
            m_render_system->push_constants(render_pass, mesh_push_constants);

            m_render_system->draw_indexed(
                render_pass, static_cast<uint32_t>(mesh.index_count), 1, static_cast<uint32_t>(mesh.start_index), 0, 0);
        }
    }

    m_render_system->end_render_pass(render_pass);

    const RenderPassId grid_render_pass = m_render_system->begin_render_pass(
        command_buffer,
        {
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
        });
    m_render_system->bind_pipeline(grid_render_pass, m_grid_pipeline);
    m_render_system->draw(grid_render_pass, 6, 1, 0, 0);
    m_render_system->end_render_pass(grid_render_pass);

    m_render_system->submit_command_buffer(command_buffer);
}

std::vector<Engine::GpuModel> Engine::upload_asset(const Asset &asset)
{
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    std::vector<GpuModel> gpu_models;

    const CommandBufferId command_buffer = m_render_system->acquire_command_buffer();
    for (const Asset::Scene &scene : asset.scenes())
    {
        for (const size_t node_index : scene.node_indices)
        {
            const Asset::Node *node = asset.nodes()[node_index].get();
            upload_model(command_buffer, asset, node, glm::mat4(1.0f), gpu_models);
        }
    }
    m_render_system->submit_command_buffer(command_buffer);

    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    HE_INFO("Asset uploaded in {:.2}s", elapsed_seconds.count());

    return gpu_models;
}

void Engine::upload_model(
    const CommandBufferId command_buffer,
    const Asset &asset,
    const Asset::Node *node,
    const glm::mat4 &parent_transform,
    std::vector<GpuModel> &models)
{
    const glm::mat4 transform = parent_transform * node->local_transform;

    if (node->model_index.has_value())
    {
        const size_t model_index = node->model_index.value();
        const Asset::Model &asset_model = asset.models()[model_index];

        const BufferId positions_buffer = m_render_system->create_buffer({
            .label = std::nullopt,
            .size = asset_model.positions.size() * sizeof(glm::vec3),
            .usage =
                {
                    BufferUsage::Storage,
                    BufferUsage::ShaderResource,
                },
        });
        m_render_system->write_buffer(
            command_buffer,
            {
                .buffer = positions_buffer,
                .offset = 0,
            },
            asset_model.positions.data(),
            asset_model.positions.size() * sizeof(glm::vec3));

        const BufferId normals_buffer = m_render_system->create_buffer({
            .label = std::nullopt,
            .size = asset_model.normals.size() * sizeof(glm::vec3),
            .usage =
                {
                    BufferUsage::Storage,
                    BufferUsage::ShaderResource,
                },
        });
        m_render_system->write_buffer(
            command_buffer,
            {
                .buffer = normals_buffer,
                .offset = 0,
            },
            asset_model.normals.data(),
            asset_model.normals.size() * sizeof(glm::vec3));

        const BufferId colors_buffer = m_render_system->create_buffer({
            .label = std::nullopt,
            .size = asset_model.colors.size() * sizeof(glm::vec3),
            .usage =
                {
                    BufferUsage::Storage,
                    BufferUsage::ShaderResource,
                },
        });
        m_render_system->write_buffer(
            command_buffer,
            {
                .buffer = colors_buffer,
                .offset = 0,
            },
            asset_model.colors.data(),
            asset_model.colors.size() * sizeof(glm::vec3));

        const BufferId uvs_buffer = m_render_system->create_buffer({
            .label = std::nullopt,
            .size = asset_model.uvs.size() * sizeof(glm::vec2),
            .usage =
                {
                    BufferUsage::Storage,
                    BufferUsage::ShaderResource,
                },
        });
        m_render_system->write_buffer(
            command_buffer,
            {
                .buffer = uvs_buffer,
                .offset = 0,
            },
            asset_model.uvs.data(),
            asset_model.uvs.size() * sizeof(glm::vec2));

        const ShaderModel shader_model = {
            .positions = m_render_system->get_buffer_handle(positions_buffer),
            .normals = m_render_system->get_buffer_handle(normals_buffer),
            .colors = m_render_system->get_buffer_handle(colors_buffer),
            .uvs = m_render_system->get_buffer_handle(uvs_buffer),
        };

        const BufferId model_buffer = m_render_system->create_buffer({
            .label = std::nullopt,
            .size = sizeof(ShaderModel),
            .usage =
                {
                    BufferUsage::Storage,
                    BufferUsage::ShaderResource,
                },
        });
        m_render_system->write_buffer(
            command_buffer,
            {
                .buffer = model_buffer,
                .offset = 0,
            },
            shader_model);

        const BufferId indices_buffer = m_render_system->create_buffer({
            .label = std::nullopt,
            .size = asset_model.indices.size() * sizeof(uint32_t),
            .usage =
                {
                    BufferUsage::Index,
                },
        });
        m_render_system->write_buffer(
            command_buffer,
            {
                .buffer = indices_buffer,
                .offset = 0,
            },
            asset_model.indices.data(),
            asset_model.indices.size() * sizeof(uint32_t));

        std::vector<GpuMesh> meshes;
        for (const Asset::Mesh &asset_mesh : asset_model.meshes)
        {
            const Asset::Material &asset_material = asset.materials()[asset_mesh.material_index];

            ResourceHandle color_texture;
            if (asset_material.base_color_texture_index.has_value())
            {
                const Asset::Texture &asset_texture = asset.textures()[asset_material.base_color_texture_index.value()];

                const TextureId texture = m_render_system->create_texture({
                    .label = std::nullopt,
                    .width = asset_texture.width,
                    .height = asset_texture.height,
                    .depth = 1,
                    .array_size = 1,
                    .mip_levels = 1,
                    .format = Format::Rgba8Srgb,
                    .dimension = Dimension::Texture2D,
                    .usage = TextureUsage::ShaderResource,
                });

                m_render_system->write_texture(
                    command_buffer,
                    {
                        .texture = texture,
                        .offset =
                            {
                                .x = 0,
                                .y = 0,
                                .z = 0,
                            },
                        .extent =
                            {
                                .width = static_cast<uint32_t>(asset_texture.width),
                                .height = static_cast<uint32_t>(asset_texture.height),
                                .depth = 1,
                            },
                        .mip_level = 0,
                        .array_index = 0,
                    },
                    asset_texture.data.data(),
                    static_cast<uint32_t>(asset_texture.width) * static_cast<uint32_t>(asset_texture.height) * asset_texture.channels);

                color_texture = m_render_system->get_texture_handle(texture);
            }
            else
            {
                color_texture = m_render_system->get_texture_handle(m_default_texture);
            }

            ResourceHandle color_sampler;
            if (asset_material.base_color_sampler_index.has_value())
            {
                const Asset::Sampler &asset_sampler = asset.samplers()[asset_material.base_color_sampler_index.value()];

                const SamplerId sampler = m_render_system->create_sampler({
                    .label = std::nullopt,
                    .mag_filter = asset_sampler.mag_filter,
                    .min_filter = asset_sampler.min_filter,
                    .mipmap_filter = asset_sampler.min_filter,
                    .address_mode_u = AddressMode::Repeat,
                    .address_mode_v = AddressMode::Repeat,
                    .address_mode_w = AddressMode::Repeat,
                    .mip_lod_bias = 0.0f,
                    .compare_operation = CompareOperation::Never,
                    .min_lod = 0.0f,
                    .max_lod = 1.0f,
                    .border_color = BorderColor::TransparentBlack,
                });

                color_sampler = m_render_system->get_sampler_handle(sampler);
            }
            else
            {
                color_sampler = m_render_system->get_sampler_handle(m_default_sampler);
            }

            ResourceHandle metal_roughness_texture;
            if (asset_material.metallic_roughness_texture_index.has_value())
            {
                const Asset::Texture &asset_texture = asset.textures()[asset_material.metallic_roughness_texture_index.value()];

                const TextureId texture = m_render_system->create_texture({
                    .label = std::nullopt,
                    .width = asset_texture.width,
                    .height = asset_texture.height,
                    .depth = 1,
                    .array_size = 1,
                    .mip_levels = 1,
                    .format = Format::Rgba8Unorm,
                    .dimension = Dimension::Texture2D,
                    .usage = TextureUsage::ShaderResource,
                });

                m_render_system->write_texture(
                    command_buffer,
                    {
                        .texture = texture,
                        .offset =
                            {
                                .x = 0,
                                .y = 0,
                                .z = 0,
                            },
                        .extent =
                            {
                                .width = static_cast<uint32_t>(asset_texture.width),
                                .height = static_cast<uint32_t>(asset_texture.height),
                                .depth = 1,
                            },
                        .mip_level = 0,
                        .array_index = 0,
                    },
                    asset_texture.data.data(),
                    static_cast<uint32_t>(asset_texture.width) * static_cast<uint32_t>(asset_texture.height) * asset_texture.channels);

                metal_roughness_texture = m_render_system->get_texture_handle(texture);
            }
            else
            {
                metal_roughness_texture = m_render_system->get_texture_handle(m_default_texture);
            }

            ResourceHandle metal_roughness_sampler;
            if (asset_material.metallic_roughness_sampler_index.has_value())
            {
                const Asset::Sampler &asset_sampler = asset.samplers()[asset_material.metallic_roughness_sampler_index.value()];

                const SamplerId sampler = m_render_system->create_sampler({
                    .label = std::nullopt,
                    .mag_filter = asset_sampler.mag_filter,
                    .min_filter = asset_sampler.min_filter,
                    .mipmap_filter = asset_sampler.min_filter,
                    .address_mode_u = AddressMode::Repeat,
                    .address_mode_v = AddressMode::Repeat,
                    .address_mode_w = AddressMode::Repeat,
                    .mip_lod_bias = 0.0f,
                    .compare_operation = CompareOperation::Never,
                    .min_lod = 0.0f,
                    .max_lod = 1.0f,
                    .border_color = BorderColor::TransparentBlack,
                });
                metal_roughness_sampler = m_render_system->get_sampler_handle(sampler);
            }
            else
            {
                metal_roughness_sampler = m_render_system->get_sampler_handle(m_default_sampler);
            }

            const ShaderMaterial shader_material = {
                .color_factors = asset_material.color_factors,
                .color_texture = color_texture,
                .color_sampler = color_sampler,
                .padding_0 = 0,
                .padding_1 = 0,
                .metal_roughness_factors = asset_material.metallic_roughness_factor,
                .metal_roughness_texture = metal_roughness_texture,
                .metal_roughness_sampler = metal_roughness_sampler,
            };

            const BufferId material_buffer = m_render_system->create_buffer({
                .label = std::nullopt,
                .size = sizeof(ShaderMaterial),
                .usage =
                    {
                        BufferUsage::Storage,
                        BufferUsage::ShaderResource,
                    },
            });
            m_render_system->write_buffer(
                command_buffer,
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