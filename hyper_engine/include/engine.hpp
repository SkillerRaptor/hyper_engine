/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "systems/input_system.hpp"
#include "systems/render/camera.hpp"
#include "systems/render_system.hpp"
#include "systems/window_system.hpp"

class Engine
{
public:
    void initialize();
    void shutdown() const;

    void run();

private:
    void fixed_update(float delta_time);
    void update(float delta_time);
    void render() const;

private:
    WindowSystem *m_window_system = nullptr;
    InputSystem *m_input_system = nullptr;
    RenderSystem *m_render_system = nullptr;

    WindowId m_window;

    Camera m_camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), -90.0f, 0.0f);
    BufferId m_camera_buffer;

    PipelineLayoutId m_pipeline_layout;

    RenderPipelineId m_grid_pipeline;
    RenderPipelineId m_render_pipeline;

    TextureId m_depth_texture;

    bool m_running = true;
};