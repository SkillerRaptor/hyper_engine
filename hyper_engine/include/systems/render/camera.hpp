/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glm/glm.hpp>

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
    Camera(glm::vec3 position, float yaw, float pitch);

    void process_keyboard(Movement movement, float delta_time);
    void process_mouse_movement(float x_position, float y_position, bool mouse_button_pressed);
    void process_mouse_scroll(float y_offset);

    void set_aspect_ratio(const float aspect_ratio) { m_aspect_ratio = aspect_ratio; };

    glm::vec3 position() const { return m_position; }

    float near_plane() const { return m_near; }
    float far_plane() const { return m_far; };

    glm::mat4 projection_matrix() const;
    glm::mat4 view_matrix() const;

private:
    void update_camera_vectors();

private:
    glm::vec3 m_position { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_front { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_up { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_right { 0.0f, 0.0f, 0.0f };

    float m_yaw { 0.0f };
    float m_pitch { 0.0f };

    float m_movement_speed { 2.5f };
    float m_mouse_sensitivity { 0.1f };
    float m_fov { 90.0f };

    float m_near { 0.01f };
    float m_far { 1000.0f };
    float m_aspect_ratio { 1280.0f / 720.0f };

    bool m_first_mouse { true };
    float m_last_x { 0.0f };
    float m_last_y { 0.0f };
};
