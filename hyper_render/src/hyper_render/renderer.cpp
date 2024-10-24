/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_render/renderer.hpp"

#include <array>

#include <glm/glm.hpp>

#include <hyper_core/filesystem.hpp>
#include <hyper_core/logger.hpp>

struct Material
{
    glm::vec4 base_color;
};

struct Mesh
{
    uint32_t positions;
    uint32_t normals;
    uint32_t padding_0;
    uint32_t padding_1;
};

struct ObjectPushConstants
{
    uint32_t mesh;
    uint32_t material;
    uint32_t padding_0;
    uint32_t padding_1;
};

namespace hyper_render
{
    static constexpr std::array<Material, 1> s_materials = {
        Material{
            .base_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        },
    };

    static constexpr std::array<glm::vec4, 4> s_positions = {
        glm::vec4(-0.5, -0.5, 0.0, 1.0),
        glm::vec4(0.5, -0.5, 0.0, 1.0),
        glm::vec4(0.5, 0.5, 0.0, 1.0),
        glm::vec4(-0.5, 0.5, 0.0, 1.0),
    };

    static constexpr std::array<glm::vec4, 4> s_normals = {
        glm::vec4(0.0, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 0.0),
    };

    static constexpr std::array<uint32_t, 6> s_indices = {
        0, 1, 2, 2, 3, 0,
    };

    Renderer::Renderer(const RendererDescriptor &descriptor)
        : m_graphics_device(descriptor.graphics_device)
        , m_surface(descriptor.surface)
        , m_command_list(m_graphics_device->create_command_list())
        , m_pipeline_layout(m_graphics_device->create_pipeline_layout({
              .label = "Opaque Pipeline Layout",
              .push_constant_size = sizeof(ObjectPushConstants),
          }))
        , m_vertex_shader(m_graphics_device->create_shader_module({
              .label = "Opaque Vertex Shader",
              .type = hyper_rhi::ShaderType::Vertex,
              .entry_name = "vs_main",
              .bytes = hyper_core::filesystem::read_file("./assets/shaders/opaque_shaders.hlsl"),
          }))
        , m_fragment_shader(m_graphics_device->create_shader_module({
              .label = "Opaque Fragment Shader",
              .type = hyper_rhi::ShaderType::Fragment,
              .entry_name = "fs_main",
              .bytes = hyper_core::filesystem::read_file("./assets/shaders/opaque_shaders.hlsl"),
          }))
        , m_pipeline(m_graphics_device->create_graphics_pipeline({
              .label = "Opaque Pipeline",
              .layout = m_pipeline_layout,
              .vertex_shader = m_vertex_shader,
              .fragment_shader = m_fragment_shader,
          }))
        , m_material_buffer(m_graphics_device->create_buffer({
              .label = "Material Buffer",
              .byte_size = sizeof(s_materials),
              .is_index_buffer = false,
              .is_constant_buffer = true,
          }))
        , m_positions_buffer(m_graphics_device->create_buffer({
              .label = "Positions Buffer",
              .byte_size = sizeof(s_positions),
              .is_index_buffer = false,
              .is_constant_buffer = true,
          }))
        , m_normals_buffer(m_graphics_device->create_buffer({
              .label = "Normals Buffer",
              .byte_size = sizeof(s_normals),
              .is_index_buffer = false,
              .is_constant_buffer = true,
          }))
        , m_mesh_buffer(m_graphics_device->create_buffer({
              .label = "Mesh Buffer",
              .byte_size = sizeof(Mesh) * 1,
              .is_index_buffer = false,
              .is_constant_buffer = true,
          }))
        , m_indices_buffer(m_graphics_device->create_buffer({
              .label = "Indices Buffer",
              .byte_size = sizeof(s_indices),
              .is_index_buffer = true,
              .is_constant_buffer = false,
          }))
        , m_frame_index(1)
    {
        m_graphics_device->wait_for_idle();

        HE_DEBUG("Created Renderer");
    }

    void Renderer::render()
    {
        m_graphics_device->begin_frame(m_surface, m_frame_index);

        m_command_list->begin();

        m_command_list->end();

        m_graphics_device->end_frame();

        m_graphics_device->execute();
        m_graphics_device->present(m_surface);

        m_frame_index += 1;
    }
} // namespace hyper_render