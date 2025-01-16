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
        std::shared_ptr<Buffer> index_buffer;

        MaterialInstance *material = nullptr;

        glm::mat4 transform;
        std::shared_ptr<Buffer> mesh_buffer;
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
        std::weak_ptr<Node> parent;
        std::vector<std::shared_ptr<Node>> children;

        glm::mat4 world_transform;
        glm::mat4 local_transform;
    };

    class MeshNode final : public Node
    {
    public:
        explicit MeshNode(const std::shared_ptr<Mesh> &mesh);

        void draw(const glm::mat4 &top_matrix, DrawContext &context) const override;

    public:
        std::shared_ptr<Mesh> mesh;
    };

    class LoadedGltf final : public Renderable
    {
    public:
        LoadedGltf(
            std::vector<std::shared_ptr<Mesh>> meshes,
            std::vector<std::shared_ptr<Node>> nodes,
            std::vector<std::shared_ptr<Texture>> textures,
            std::vector<std::shared_ptr<TextureView>> texture_views,
            std::vector<std::shared_ptr<GltfMaterial>> materials,
            std::vector<std::shared_ptr<Node>> top_nodes,
            std::vector<std::shared_ptr<Sampler>> samplers);

        void draw(const glm::mat4 &top_matrix, DrawContext &draw_context) const override;

    private:
        std::vector<std::shared_ptr<Mesh>> m_meshes;
        std::vector<std::shared_ptr<Node>> m_nodes;
        std::vector<std::shared_ptr<Texture>> m_textures;
        std::vector<std::shared_ptr<TextureView>> m_texture_views;
        std::vector<std::shared_ptr<GltfMaterial>> m_materials;
        std::vector<std::shared_ptr<Node>> m_top_nodes;
        std::vector<std::shared_ptr<Sampler>> m_samplers;
    };

    std::shared_ptr<LoadedGltf> load_gltf(
        const std::shared_ptr<CommandList> &command_list,
        const std::shared_ptr<TextureView> &white_texture_view,
        const std::shared_ptr<Texture> &error_texture,
        const std::shared_ptr<TextureView> &error_texture_view,
        const std::shared_ptr<Sampler> &default_sampler_linear,
        const GltfMetallicRoughness &metallic_roughness_material,
        const std::string &path);
} // namespace hyper_engine