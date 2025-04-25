/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "scene/resources/asset.hpp"
#include "systems/input_system.hpp"
#include "systems/render/camera.hpp"
#include "systems/render_system.hpp"
#include "systems/window_system.hpp"

class Engine
{
private:
    struct GpuMesh
    {
        size_t start_index = 0;
        size_t index_count = 0;

        BufferId material_buffer;
    };

    struct GpuModel
    {
        glm::mat4 transform = glm::mat4(1.0f);
        BufferId model_buffer;
        BufferId indices_buffer;

        std::vector<GpuMesh> meshes;
    };

public:
    void initialize();
    void shutdown() const;

    void run();

private:
    void fixed_update(float delta_time);
    void update(float delta_time);
    void render() const;

    std::vector<GpuModel> upload_asset(const Asset &asset);
    void upload_model(
        CommandBufferId command_buffer,
        const Asset &asset,
        const Asset::Node *node,
        const glm::mat4 &parent_transform,
        std::vector<GpuModel> &models);

private:
    WindowSystem *m_window_system = nullptr;
    InputSystem *m_input_system = nullptr;
    RenderSystem *m_render_system = nullptr;

    WindowId m_window;

    Camera m_camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), -90.0f, 0.0f);
    BufferId m_camera_buffer;
    BufferId m_scene_buffer;

    TextureId m_default_texture;
    SamplerId m_default_sampler;

    PipelineLayoutId m_pipeline_layout;
    RenderPipelineId m_grid_pipeline;
    RenderPipelineId m_render_pipeline;
    TextureId m_depth_texture;

    Asset m_sponza;
    std::vector<GpuModel> m_renderables;

    bool m_running = true;
};