/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glm/glm.hpp>

namespace hyper_engine
{
    struct CameraData
    {
        glm::vec3 position;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        float near_plane = 0.1f;
        float far_plane = 1000.0f;
    };
} // namespace hyper_engine