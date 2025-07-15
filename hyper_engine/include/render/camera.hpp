/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glm/glm.hpp>

#include "core/types.hpp"

class Camera
{
public:
    enum class Movement
    {
        Forward,
        Backward,
        Left,
        Right,
    };

public:
    Camera(glm::vec3 position, f32 yaw, f32 pitch);

    void process_keyboard(Movement, f32 delta_time);
    void process_mouse_movement(f32 x_position, f32 y_position, bool mouse_button_pressed);
    void process_mouse_scroll(f32 y_offset);

    void set_aspect_ratio(const f32 aspect_ratio) { m_aspect_ratio = aspect_ratio; };

    glm::vec3 position() const { return m_position; }

    f32 near_plane() const { return m_near; }
    f32 far_plane() const { return m_far; };

    glm::mat4 projection_matrix() const;
    glm::mat4 view_matrix() const;

private:
    void update_camera_vectors();

private:
    glm::vec3 m_position { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_front { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_up { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_right { 0.0f, 0.0f, 0.0f };

    f32 m_yaw { 0.0f };
    f32 m_pitch { 0.0f };

    f32 m_movement_speed { 2.5f };
    f32 m_mouse_sensitivity { 0.1f };
    f32 m_fov { 90.0f };

    f32 m_near { 0.01f };
    f32 m_far { 1000.0f };
    f32 m_aspect_ratio { 1280.0f / 720.0f };

    bool m_first_mouse { true };
    f32 m_last_x { 0.0f };
    f32 m_last_y { 0.0f };
};
