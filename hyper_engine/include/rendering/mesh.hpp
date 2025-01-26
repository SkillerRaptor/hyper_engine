/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <hyper_rhi/forward.hpp>

#include "hyper_rendering/material.hpp"

namespace hyper_engine
{
    struct GltfMaterial
    {
        MaterialInstance data;
    };

    struct GltfSurface
    {
        uint32_t start_index = 0;
        uint32_t count = 0;
        std::shared_ptr<GltfMaterial> material;
    };

    class Mesh
    {
    public:
        Mesh(
            std::string name,
            std::vector<GltfSurface> surfaces,
            std::shared_ptr<Buffer> positions_buffer,
            std::shared_ptr<Buffer> normals_buffer,
            std::shared_ptr<Buffer> colors_buffer,
            std::shared_ptr<Buffer> tex_coords_buffer,
            std::shared_ptr<Buffer> mesh_buffer,
            std::shared_ptr<Buffer> indices_buffer);

        std::string_view name() const;

        const std::vector<GltfSurface> &surfaces() const;
        std::shared_ptr<Buffer> positions_buffer() const;
        std::shared_ptr<Buffer> normals_buffer() const;
        std::shared_ptr<Buffer> colors_buffer() const;
        std::shared_ptr<Buffer> tex_coords_buffer() const;
        std::shared_ptr<Buffer> mesh_buffer() const;
        std::shared_ptr<Buffer> indices_buffer() const;

    private:
        std::string m_name;
        std::vector<GltfSurface> m_surfaces;

        std::shared_ptr<Buffer> m_positions_buffer;
        std::shared_ptr<Buffer> m_normals_buffer;
        std::shared_ptr<Buffer> m_colors_buffer;
        std::shared_ptr<Buffer> m_tex_coords_buffer;

        std::shared_ptr<Buffer> m_mesh_buffer;
        std::shared_ptr<Buffer> m_indices_buffer;
    };
} // namespace hyper_engine