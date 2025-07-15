/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include "platform/event_server.hpp"
#include "platform/input_server.hpp"
#include "platform/window_server.hpp"
#include "render/camera.hpp"
#include "render/render_server.hpp"
#include "scene/resources/asset.hpp"

class Engine
{
private:
    struct GpuMesh
    {
        size_t start_index { 0 };
        size_t index_count { 0 };

        BufferId material_buffer {};
    };

    struct GpuModel
    {
        glm::mat4 transform { 1.0f };
        BufferId model_buffer {};
        BufferId indices_buffer {};

        std::vector<GpuMesh> meshes {};
    };

public:
    void initialize();
    void shutdown() const;

    void run();

private:
    void fixed_update(f32 delta_time);
    void update(f32 delta_time);
    void render() const;

    void create_pbr();
    void create_skybox();
    void create_grid();
    void create_composition();
    void create_default();

    std::vector<GpuModel> upload_asset(const Asset &);
    void upload_model(
        CommandBufferId, const Asset &, const Asset::Node *, const glm::mat4 &parent_transform, std::vector<GpuModel> &);

private:
    std::unique_ptr<WindowServer> m_window_server { nullptr };
    WindowId m_window {};

    std::unique_ptr<EventServer> m_event_server { nullptr };
    std::unique_ptr<InputServer> m_input_server { nullptr };

    std::unique_ptr<RenderServer> m_render_server { nullptr };

    Camera m_camera { glm::vec3(0.0f, 0.0f, 3.0f), -90.0f, 0.0f };
    BufferId m_camera_buffer {};
    BufferId m_scene_buffer {};

    PipelineLayoutId m_pbr_layout {};
    RenderPipelineId m_pbr_pipeline {};

    /*
    PipelineLayoutId m_skybox_layout;
    RenderPipelineId m_skybox_pipeline;
    TextureId m_skybox_texture;
    SamplerId m_skybox_sampler;
    */

    RenderPipelineId m_grid_pipeline {};

    PipelineLayoutId m_composition_layout {};
    RenderPipelineId m_composition_pipeline {};
    TextureId m_composition_texture {};
    TextureViewId m_composition_texture_view {};
    SamplerId m_composition_sampler {};
    TextureId m_depth_texture {};
    TextureViewId m_depth_texture_view {};

    TextureId m_default_texture {};
    TextureViewId m_default_texture_view {};
    SamplerId m_default_sampler {};

    Asset m_sponza;
    std::vector<GpuModel> m_renderables {};

    bool m_running { true };
};
