/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>

#include <hyper_core/ref_ptr.hpp>
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
        RefPtr<GltfMaterial> material;
    };

    class Mesh
    {
    public:
        Mesh(
            std::string name,
            std::vector<GltfSurface> surfaces,
            RefPtr<Buffer> positions_buffer,
            RefPtr<Buffer> normals_buffer,
            RefPtr<Buffer> colors_buffer,
            RefPtr<Buffer> tex_coords_buffer,
            RefPtr<Buffer> mesh_buffer,
            RefPtr<Buffer> indices_buffer);

        std::string_view name() const;

        const std::vector<GltfSurface> &surfaces() const;
        RefPtr<Buffer> positions_buffer() const;
        RefPtr<Buffer> normals_buffer() const;
        RefPtr<Buffer> colors_buffer() const;
        RefPtr<Buffer> tex_coords_buffer() const;
        RefPtr<Buffer> mesh_buffer() const;
        RefPtr<Buffer> indices_buffer() const;

    private:
        std::string m_name;
        std::vector<GltfSurface> m_surfaces;

        RefPtr<Buffer> m_positions_buffer;
        RefPtr<Buffer> m_normals_buffer;
        RefPtr<Buffer> m_colors_buffer;
        RefPtr<Buffer> m_tex_coords_buffer;

        RefPtr<Buffer> m_mesh_buffer;
        RefPtr<Buffer> m_indices_buffer;
    };
} // namespace hyper_engine