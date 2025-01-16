/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/mesh.hpp"

#include <hyper_rhi/buffer.hpp>

namespace hyper_engine
{
    Mesh::Mesh(
        std::string name,
        std::vector<GltfSurface> surfaces,
        RefPtr<Buffer> positions_buffer,
        RefPtr<Buffer> normals_buffer,
        RefPtr<Buffer> colors_buffer,
        RefPtr<Buffer> tex_coords_buffer,
        RefPtr<Buffer> mesh_buffer,
        RefPtr<Buffer> indices_buffer)
        : m_name(std::move(name))
        , m_surfaces(std::move(surfaces))
        , m_positions_buffer(std::move(positions_buffer))
        , m_normals_buffer(std::move(normals_buffer))
        , m_colors_buffer(std::move(colors_buffer))
        , m_tex_coords_buffer(std::move(tex_coords_buffer))
        , m_mesh_buffer(std::move(mesh_buffer))
        , m_indices_buffer(std::move(indices_buffer))
    {
    }

    std::string_view Mesh::name() const
    {
        return m_name;
    }

    const std::vector<GltfSurface> &Mesh::surfaces() const
    {
        return m_surfaces;
    }

    RefPtr<Buffer> Mesh::positions_buffer() const
    {
        return m_positions_buffer;
    }

    RefPtr<Buffer> Mesh::normals_buffer() const
    {
        return m_normals_buffer;
    }

    RefPtr<Buffer> Mesh::colors_buffer() const
    {
        return m_colors_buffer;
    }

    RefPtr<Buffer> Mesh::tex_coords_buffer() const
    {
        return m_tex_coords_buffer;
    }

    RefPtr<Buffer> Mesh::mesh_buffer() const
    {
        return m_mesh_buffer;
    }

    RefPtr<Buffer> Mesh::indices_buffer() const
    {
        return m_indices_buffer;
    }
} // namespace hyper_engine