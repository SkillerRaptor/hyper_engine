/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/renderer.hpp"

#include <array>

#include <hyper_core/logger.hpp>
#include <hyper_rhi/buffer.hpp>
#include <hyper_rhi/command_list.hpp>
#include <hyper_rhi/graphics_device.hpp>
#include <hyper_rhi/sampler.hpp>
#include <hyper_rhi/surface.hpp>
#include <hyper_rhi/texture.hpp>
#include <hyper_rhi/texture_view.hpp>

#include "hyper_rendering/material.hpp"
#include "hyper_rendering/render_passes/grid_pass.hpp"
#include "hyper_rendering/render_passes/opaque_pass.hpp"

#include "shader_interop.h"

namespace hyper_engine
{
    Renderer::Renderer(GraphicsDevice &graphics_device, SDL_Window *window)
        : m_graphics_device(graphics_device)
        , m_surface(m_graphics_device.create_surface(window))
        , m_command_list(m_graphics_device.create_command_list())
        , m_render_texture(m_graphics_device.create_texture({
              .label = "Render",
              .width = m_surface->width(),
              .height = m_surface->height(),
              .depth = 1,
              .array_size = 1,
              .mip_levels = 1,
              .format = Format::Bgra8Unorm,
              .dimension = Dimension::Texture2D,
              .usage = TextureUsage::RenderAttachment,
          }))
        , m_render_texture_view(m_graphics_device.create_texture_view({
              .label = "Render",
              .texture = m_render_texture,
              .subresource_range =
                  {
                      .base_mip_level = 0,
                      .mip_level_count = 1,
                      .base_array_level = 0,
                      .array_layer_count = 1,
                  },
              .component_mapping =
                  {
                      .r = ComponentSwizzle::Identity,
                      .g = ComponentSwizzle::Identity,
                      .b = ComponentSwizzle::Identity,
                      .a = ComponentSwizzle::Identity,
                  },
          }))
        , m_depth_texture(m_graphics_device.create_texture({
              .label = "Depth",
              .width = m_surface->width(),
              .height = m_surface->height(),
              .depth = 1,
              .array_size = 1,
              .mip_levels = 1,
              .format = Format::D32Sfloat,
              .dimension = Dimension::Texture2D,
              .usage = TextureUsage::RenderAttachment,
          }))
        , m_depth_texture_view(m_graphics_device.create_texture_view({
              .label = "Depth",
              .texture = m_depth_texture,
              .subresource_range =
                  {
                      .base_mip_level = 0,
                      .mip_level_count = 1,
                      .base_array_level = 0,
                      .array_layer_count = 1,
                  },
              .component_mapping =
                  {
                      .r = ComponentSwizzle::Identity,
                      .g = ComponentSwizzle::Identity,
                      .b = ComponentSwizzle::Identity,
                      .a = ComponentSwizzle::Identity,
                  },
          }))
        , m_camera_buffer(m_graphics_device.create_buffer(
              {
                  .label = "Camera",
                  .byte_size = sizeof(ShaderCamera),
                  .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
              },
              ResourceHandle(HE_DESCRIPTOR_SET_SLOT_CAMERA)))
        , m_scene_buffer(m_graphics_device.create_buffer({
              .label = "Scene",
              .byte_size = sizeof(ShaderScene),
              .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
          }))
        , m_white_texture(m_graphics_device.create_texture({
              .label = "White",
              .width = 1,
              .height = 1,
              .depth = 1,
              .array_size = 1,
              .mip_levels = 1,
              .format = Format::Rgba8Unorm,
              .dimension = Dimension::Texture2D,
              .usage = TextureUsage::ShaderResource,
          }))
        , m_white_texture_view(m_graphics_device.create_texture_view({
              .label = "White",
              .texture = m_white_texture,
              .subresource_range =
                  {
                      .base_mip_level = 0,
                      .mip_level_count = 1,
                      .base_array_level = 0,
                      .array_layer_count = 1,
                  },
              .component_mapping =
                  {
                      .r = ComponentSwizzle::Identity,
                      .g = ComponentSwizzle::Identity,
                      .b = ComponentSwizzle::Identity,
                      .a = ComponentSwizzle::Identity,
                  },
          }))
        , m_error_texture(m_graphics_device.create_texture({
              .label = "Error",
              .width = 16,
              .height = 16,
              .depth = 1,
              .array_size = 1,
              .mip_levels = 1,
              .format = Format::Rgba8Unorm,
              .dimension = Dimension::Texture2D,
              .usage = TextureUsage::ShaderResource,
          }))
        , m_error_texture_view(m_graphics_device.create_texture_view({
              .label = "Error",
              .texture = m_error_texture,
              .subresource_range =
                  {
                      .base_mip_level = 0,
                      .mip_level_count = 1,
                      .base_array_level = 0,
                      .array_layer_count = 1,
                  },
              .component_mapping =
                  {
                      .r = ComponentSwizzle::Identity,
                      .g = ComponentSwizzle::Identity,
                      .b = ComponentSwizzle::Identity,
                      .a = ComponentSwizzle::Identity,
                  },
          }))
        , m_default_sampler_nearest(m_graphics_device.create_sampler({
              .label = "Default Nearest",
              .mag_filter = Filter::Nearest,
              .min_filter = Filter::Nearest,
              .mipmap_filter = Filter::Nearest,
              .address_mode_u = AddressMode::Repeat,
              .address_mode_v = AddressMode::Repeat,
              .address_mode_w = AddressMode::Repeat,
              .mip_lod_bias = 0.0,
              .compare_operation = CompareOperation::Never,
              .min_lod = 0.0,
              .max_lod = 1.0,
              .border_color = BorderColor::TransparentBlack,
          }))
        , m_default_sampler_linear(m_graphics_device.create_sampler({
              .label = "Default Linear",
              .mag_filter = Filter::Linear,
              .min_filter = Filter::Linear,
              .mipmap_filter = Filter::Nearest,
              .address_mode_u = AddressMode::Repeat,
              .address_mode_v = AddressMode::Repeat,
              .address_mode_w = AddressMode::Repeat,
              .mip_lod_bias = 0.0,
              .compare_operation = CompareOperation::Never,
              .min_lod = 0.0,
              .max_lod = 1.0,
              .border_color = BorderColor::TransparentBlack,
          }))
        , m_metallic_roughness_material(m_graphics_device, m_shader_compiler, m_render_texture, m_depth_texture)
    {
        const GltfMetallicRoughness::MaterialResources material_resources = {
            .color_factors = glm::vec4(1.0, 1.0, 1.0, 1.0),
            .color_texture_view = m_white_texture_view,
            .color_sampler = m_default_sampler_linear,
            .metal_roughness_factors = glm::vec4(1.0, 0.5, 0.0, 0.0),
            .metal_roughness_texture_view = m_white_texture_view,
            .metal_roughness_sampler = m_default_sampler_linear,
        };

        m_command_list->begin();

        const uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
        // FIXME: Automate barrier
        m_command_list->insert_barriers({
            .memory_barriers = {},
            .buffer_memory_barriers = {},
            .texture_memory_barriers =
                {
                    {
                        .stage_before = BarrierPipelineStage::AllCommands,
                        .stage_after = BarrierPipelineStage::AllCommands,
                        .access_before = BarrierAccess::None,
                        .access_after = BarrierAccess::TransferWrite,
                        .layout_before = BarrierTextureLayout::Undefined,
                        .layout_after = BarrierTextureLayout::TransferDst,
                        .texture = m_white_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                },
        });
        m_command_list->write_texture(
            m_white_texture,
            {
                .x = 0,
                .y = 0,
                .z = 0,
            },
            {
                .width = 1,
                .height = 1,
                .depth = 1,
            },
            0,
            0,
            &white,
            sizeof(white),
            0);

        const uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
        const uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));

        std::array<uint32_t, 16 * 16> pixels = {};
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 16; y++)
            {
                pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }

        // FIXME: Automate barrier
        m_command_list->insert_barriers({
            .memory_barriers = {},
            .buffer_memory_barriers = {},
            .texture_memory_barriers =
                {
                    {
                        .stage_before = BarrierPipelineStage::AllCommands,
                        .stage_after = BarrierPipelineStage::AllCommands,
                        .access_before = BarrierAccess::None,
                        .access_after = BarrierAccess::TransferWrite,
                        .layout_before = BarrierTextureLayout::Undefined,
                        .layout_after = BarrierTextureLayout::TransferDst,
                        .texture = m_error_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                },
        });
        m_command_list->write_texture(
            m_error_texture,
            {
                .x = 0,
                .y = 0,
                .z = 0,
            },
            {
                .width = 16,
                .height = 16,
                .depth = 1,
            },
            0,
            0,
            &pixels,
            sizeof(pixels),
            0);

        MaterialInstance default_data =
            m_metallic_roughness_material.write_material(m_command_list, MaterialPassType::MainColor, material_resources);

        const std::shared_ptr<LoadedGltf> scene = load_gltf(
            m_command_list,
            m_white_texture_view,
            m_error_texture,
            m_error_texture_view,
            m_default_sampler_linear,
            m_metallic_roughness_material,
            "./assets/models/DamagedHelmet.glb");
        m_scenes["DamagedHelmet"] = scene;

        // FIXME: ShaderScene shouldn't be fixed and should actually contain meaningful data
        constexpr ShaderScene shader_scene = {
            .ambient_color = glm::vec4(0.1f),
            .sunlight_direction = glm::vec4(0.0f, 1.0f, 0.5f, 1.0f),
            .sunlight_color = glm::vec4(1.0f),
            .padding_0 = glm::vec4(0.0f),
        };

        m_command_list->write_buffer(m_scene_buffer, &shader_scene, sizeof(ShaderScene), 0);

        m_command_list->end();

        m_graphics_device.execute(m_command_list);
        m_graphics_device.wait_for_idle();

        m_opaque_pass = std::make_unique<OpaquePass>(m_render_texture_view, m_depth_texture_view, m_scene_buffer);

        m_grid_pass = std::make_unique<GridPass>(
            m_graphics_device, m_shader_compiler, m_render_texture, m_render_texture_view, m_depth_texture, m_depth_texture_view);

        // m_imgui_pass = make<ImGuiPass>(m_surface);

        // HE_INFO("Created Renderer");
    }

    Renderer::~Renderer() = default;

    void Renderer::begin_frame(const CameraData &camera)
    {
        // NOTE: Preparing scene
        m_command_list->begin();

        const glm::mat4 view_matrix = camera.view;
        const glm::mat4 projection_matrix = camera.projection;
        const glm::mat4 view_projection_matrix = projection_matrix * view_matrix;
        const ShaderCamera shader_camera = {
            .position = glm::vec4(camera.position, 1.0),
            .view = camera.view,
            .inverse_view = glm::inverse(view_matrix),
            .projection = projection_matrix,
            .inverse_projection = glm::inverse(projection_matrix),
            .view_projection = view_projection_matrix,
            .inverse_view_projection = glm::inverse(view_projection_matrix),
            .near_plane = camera.near_plane,
            .far_plane = camera.far_plane,
            .padding_0 = 0.0,
            .padding_1 = 0.0,
        };

        m_command_list->write_buffer(m_camera_buffer, &shader_camera, sizeof(ShaderCamera), 0);

        m_command_list->end();

        m_graphics_device.execute(m_command_list);
        m_graphics_device.wait_for_idle();

        m_graphics_device.begin_frame(m_surface, m_frame_index);

        m_command_list->begin();
    }

    void Renderer::end_frame()
    {
        m_command_list->end();

        m_graphics_device.end_frame();

        m_frame_index += 1;
    }

    void Renderer::present() const
    {
        m_graphics_device.execute(m_command_list);
        m_graphics_device.present(m_surface);
    }

    void Renderer::render_scene(const Scene &scene)
    {
        m_draw_context.opaque_surfaces.clear();
        m_draw_context.transparent_surfaces.clear();

        // NOTE: Here we are appending the current models to the render list
        /*
        const auto view = scene.registry().view<const TransformComponent, const ModelComponent>();
        view.each(
            [this](const TransformComponent &transform, const ModelComponent &model)
            {
                glm::mat4 model_matrix = glm::mat4(1.0f);
                model_matrix = glm::scale(model_matrix, transform.scale);
                // FIXME: Add rotation
                model_matrix = glm::translate(model_matrix, transform.translation);

                // FIXME: Don't hardcode the model
                m_scenes["DamagedHelmet"]->draw(model_matrix, m_draw_context);
            });
        */

        // NOTE: The rendering should be in the order of
        // 1. Opaque Pass
        // 2. If there is debug draw data, then render those
        // 3. If the editor enabled the grid, then do the grid pass
        // 3. If the editor enabled the gui, then do the gui pass

        // FIXME: Add automatic barriers
        // FIXME: Add debug renderer
        // FIXME: Toggle grid & ui

        // Transition render texture to color attachment and depth texture to depth stencil attachment
        m_command_list->insert_barriers({
            .memory_barriers = {},
            .buffer_memory_barriers = {},
            .texture_memory_barriers =
                {
                    {
                        .stage_before = BarrierPipelineStage::AllCommands,
                        .stage_after = BarrierPipelineStage::ColorAttachmentOutput,
                        .access_before = BarrierAccess::None,
                        .access_after = BarrierAccess::ColorAttachmentWrite,
                        .layout_before = BarrierTextureLayout::Undefined,
                        .layout_after = BarrierTextureLayout::ColorAttachment,
                        .texture = m_render_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                    {
                        .stage_before = BarrierPipelineStage::AllCommands,
                        .stage_after = BarrierPipelineStage::LateFragmentTests,
                        .access_before = BarrierAccess::None,
                        .access_after = BarrierAccess::DepthStencilAttachmentWrite,
                        .layout_before = BarrierTextureLayout::Undefined,
                        .layout_after = BarrierTextureLayout::DepthStencilAttachment,
                        .texture = m_depth_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                },
        });

        m_opaque_pass->render(m_command_list, m_draw_context);

        // NOTE: Ensure depth image was written
        m_command_list->insert_barriers({
            .memory_barriers = {},
            .buffer_memory_barriers = {},
            .texture_memory_barriers =
                {
                    {
                        .stage_before = BarrierPipelineStage::LateFragmentTests,
                        .stage_after = BarrierPipelineStage::EarlyFragmentTests,
                        .access_before = BarrierAccess::DepthStencilAttachmentWrite,
                        .access_after = BarrierAccess::DepthStencilAttachmentWrite,
                        .layout_before = BarrierTextureLayout::DepthStencilAttachment,
                        .layout_after = BarrierTextureLayout::DepthStencilAttachment,
                        .texture = m_depth_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                },
        });

        m_grid_pass->render(m_command_list);

        // FIXME: Add error if current texture is asked before the frame began
        const std::shared_ptr<Texture> swapchain_texture = m_surface->current_texture();
        const std::shared_ptr<TextureView> swapchain_texture_view = m_surface->current_texture_view();

        // NOTE: Transitioning the render and swapchain image for transfer
        m_command_list->insert_barriers({
            .memory_barriers = {},
            .buffer_memory_barriers = {},
            .texture_memory_barriers =
                {
                    {
                        .stage_before = BarrierPipelineStage::ColorAttachmentOutput,
                        .stage_after = BarrierPipelineStage::AllTransfer,
                        .access_before = BarrierAccess::ColorAttachmentWrite,
                        .access_after = BarrierAccess::TransferRead,
                        .layout_before = BarrierTextureLayout::ColorAttachment,
                        .layout_after = BarrierTextureLayout::TransferSrc,
                        .texture = m_render_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                    {
                        .stage_before = BarrierPipelineStage::AllCommands,
                        .stage_after = BarrierPipelineStage::AllTransfer,
                        .access_before = BarrierAccess::None,
                        .access_after = BarrierAccess::TransferWrite,
                        .layout_before = BarrierTextureLayout::Undefined,
                        .layout_after = BarrierTextureLayout::TransferDst,
                        .texture = swapchain_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                },
        });

        m_command_list->copy_texture_to_texture(
            m_render_texture,
            {
                .x = 0,
                .y = 0,
                .z = 0,
            },
            0,
            0,
            swapchain_texture,
            {
                .x = 0,
                .y = 0,
                .z = 0,
            },
            0,
            0,
            {
                .width = m_render_texture->width(),
                .height = m_render_texture->height(),
                .depth = m_render_texture->depth(),
            });

        m_command_list->insert_barriers({
            .memory_barriers = {},
            .buffer_memory_barriers = {},
            .texture_memory_barriers =
                {
                    {
                        .stage_before = BarrierPipelineStage::AllTransfer,
                        .stage_after = BarrierPipelineStage::ColorAttachmentOutput,
                        .access_before = BarrierAccess::TransferWrite,
                        .access_after = BarrierAccess::ColorAttachmentWrite,
                        .layout_before = BarrierTextureLayout::TransferDst,
                        .layout_after = BarrierTextureLayout::ColorAttachment,
                        .texture = swapchain_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                },
        });

        // m_imgui_pass->render(m_command_list, swapchain_texture_view);

        m_command_list->insert_barriers({
            .memory_barriers = {},
            .buffer_memory_barriers = {},
            .texture_memory_barriers =
                {
                    {
                        .stage_before = BarrierPipelineStage::ColorAttachmentOutput,
                        .stage_after = BarrierPipelineStage::AllCommands,
                        .access_before = BarrierAccess::ColorAttachmentWrite,
                        .access_after = BarrierAccess::None,
                        .layout_before = BarrierTextureLayout::ColorAttachment,
                        .layout_after = BarrierTextureLayout::Present,
                        .texture = swapchain_texture,
                        .subresource_range =
                            {
                                .base_mip_level = 0,
                                .mip_level_count = 1,
                                .base_array_level = 0,
                                .array_layer_count = 1,
                            },
                    },
                },
        });
    }

    void Renderer::create_textures(const uint32_t width, const uint32_t height)
    {
        m_render_texture = m_graphics_device.create_texture({
            .label = "Render",
            .width = width,
            .height = height,
            .depth = 1,
            .array_size = 1,
            .mip_levels = 1,
            .format = Format::Bgra8Unorm,
            .dimension = Dimension::Texture2D,
            .usage = TextureUsage::RenderAttachment,
        });

        m_render_texture_view = m_graphics_device.create_texture_view({
            .label = "Render",
            .texture = m_render_texture,
            .subresource_range =
                {
                    .base_mip_level = 0,
                    .mip_level_count = 1,
                    .base_array_level = 0,
                    .array_layer_count = 1,
                },
            .component_mapping =
                {
                    .r = ComponentSwizzle::Identity,
                    .g = ComponentSwizzle::Identity,
                    .b = ComponentSwizzle::Identity,
                    .a = ComponentSwizzle::Identity,
                },
        });

        m_depth_texture = m_graphics_device.create_texture({
            .label = "Depth",
            .width = width,
            .height = height,
            .depth = 1,
            .array_size = 1,
            .mip_levels = 1,
            .format = Format::D32Sfloat,
            .dimension = Dimension::Texture2D,
            .usage = TextureUsage::RenderAttachment,
        });

        m_depth_texture_view = m_graphics_device.create_texture_view({
            .label = "Depth",
            .texture = m_depth_texture,
            .subresource_range =
                {
                    .base_mip_level = 0,
                    .mip_level_count = 1,
                    .base_array_level = 0,
                    .array_layer_count = 1,
                },
            .component_mapping =
                {
                    .r = ComponentSwizzle::Identity,
                    .g = ComponentSwizzle::Identity,
                    .b = ComponentSwizzle::Identity,
                    .a = ComponentSwizzle::Identity,
                },
        });
    }

    /*
    void Renderer::on_resize(const WindowResizeEvent &event)
    {
        m_surface->resize(event.width(), event.height());

        create_textures(event.width(), event.height());
    }
    */
} // namespace hyper_engine