/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <hyper_rhi/forward.hpp>

#include "hyper_rendering/forward.hpp"
#include "hyper_rendering/material.hpp"
#include "hyper_rendering/mesh.hpp"

namespace hyper_engine
{
    struct RenderObject
    {
        uint32_t index_count = 0;
        uint32_t first_index = 0;
        RefPtr<Buffer> index_buffer;

        MaterialInstance *material = nullptr;

        glm::mat4 transform;
        RefPtr<Buffer> mesh_buffer;
    };

    struct DrawContext
    {
        std::vector<RenderObject> opaque_surfaces;
        std::vector<RenderObject> transparent_surfaces;
    };

    class Renderable
    {
    public:
        virtual ~Renderable() = default;

        virtual void draw(const glm::mat4 &top_matrix, DrawContext &draw_context) const = 0;
    };

    class Node : public Renderable
    {
    public:
        void refresh_transform(const glm::mat4 &parent_matrix);

        void draw(const glm::mat4 &top_matrix, DrawContext &context) const override;

    public:
        WeakPtr<Node> parent;
        std::vector<RefPtr<Node>> children;

        glm::mat4 world_transform;
        glm::mat4 local_transform;
    };

    class MeshNode final : public Node
    {
    public:
        explicit MeshNode(const RefPtr<Mesh> &mesh);

        void draw(const glm::mat4 &top_matrix, DrawContext &context) const override;

    public:
        RefPtr<Mesh> mesh;
    };

    class LoadedGltf final : public Renderable
    {
    public:
        LoadedGltf(
            std::vector<RefPtr<Mesh>> meshes,
            std::vector<RefPtr<Node>> nodes,
            std::vector<RefPtr<Texture>> textures,
            std::vector<RefPtr<TextureView>> texture_views,
            std::vector<RefPtr<GltfMaterial>> materials,
            std::vector<RefPtr<Node>> top_nodes,
            std::vector<RefPtr<Sampler>> samplers);

        void draw(const glm::mat4 &top_matrix, DrawContext &draw_context) const override;

    private:
        std::vector<RefPtr<Mesh>> m_meshes;
        std::vector<RefPtr<Node>> m_nodes;
        std::vector<RefPtr<Texture>> m_textures;
        std::vector<RefPtr<TextureView>> m_texture_views;
        std::vector<RefPtr<GltfMaterial>> m_materials;
        std::vector<RefPtr<Node>> m_top_nodes;
        std::vector<RefPtr<Sampler>> m_samplers;
    };

    RefPtr<LoadedGltf> load_gltf(
        const RefPtr<CommandList> &command_list,
        const RefPtr<TextureView> &white_texture_view,
        const RefPtr<Texture> &error_texture,
        const RefPtr<TextureView> &error_texture_view,
        const RefPtr<Sampler> &default_sampler_linear,
        const GltfMetallicRoughness &metallic_roughness_material,
        const std::string &path);
} // namespace hyper_engine