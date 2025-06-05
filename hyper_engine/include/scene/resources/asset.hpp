/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>

#include "systems/render/render_types.hpp"

class Asset
{
public:
    struct Sampler
    {
        Filter mag_filter { Filter::Nearest };
        Filter min_filter { Filter::Nearest };
    };

    struct Texture
    {
        uint32_t width { 0 };
        uint32_t height { 0 };
        uint8_t channels { 0 };
        std::vector<uint8_t> data {};
    };

    enum class AlphaMode
    {
        Opaque,
        Transparent,
    };

    struct Material
    {
        glm::vec4 color_factors { 0.0f };
        std::optional<size_t> base_color_texture_index { std::nullopt };
        std::optional<size_t> base_color_sampler_index { std::nullopt };

        glm::vec2 metallic_roughness_factor { 0.0f };
        std::optional<size_t> metallic_roughness_texture_index { std::nullopt };
        std::optional<size_t> metallic_roughness_sampler_index { std::nullopt };

        std::optional<size_t> normal_texture_index { std::nullopt };
        std::optional<size_t> normal_sampler_index { std::nullopt };
        float normal_scale { 1.0f };

        AlphaMode alpha_mode { AlphaMode::Opaque };
        float alpha_cutoff { 0.0f };

        bool double_sided { false };
    };

    struct Mesh
    {
        size_t start_index { 0 };
        size_t index_count { 0 };
        size_t material_index { 0 };
    };

    struct Model
    {
        std::vector<glm::vec3> positions {};
        std::vector<glm::vec3> normals {};
        std::vector<glm::vec4> tangents {};
        std::vector<glm::vec3> colors {};
        std::vector<glm::vec2> uvs {};
        std::vector<uint32_t> indices {};
        std::vector<Mesh> meshes {};
    };

    struct Node
    {
        Node *parent { nullptr };
        std::vector<Node *> children {};

        glm::mat4 local_transform { 1.0 };
        std::optional<size_t> model_index { std::nullopt };
    };

    struct Scene
    {
        std::vector<size_t> node_indices {};
    };

public:
    Asset() = default;

    static Asset load(std::string_view path);

    std::span<const Sampler> samplers() const { return m_samplers; };
    std::span<const Texture> textures() const { return m_textures; };
    std::span<const Material> materials() const { return m_materials; };
    std::span<const Model> models() const { return m_models; };
    std::span<const std::unique_ptr<Node>> nodes() const { return m_nodes; };
    std::span<const Scene> scenes() const { return m_scenes; }

private:
    Asset(
        std::vector<Sampler> samplers,
        std::vector<Texture> textures,
        std::vector<Material> materials,
        std::vector<Model> models,
        std::vector<std::unique_ptr<Node>> nodes,
        std::vector<Scene> scenes);

private:
    std::vector<Sampler> m_samplers {};
    std::vector<Texture> m_textures {};
    std::vector<Material> m_materials {};
    std::vector<Model> m_models {};
    std::vector<std::unique_ptr<Node>> m_nodes {};
    std::vector<Scene> m_scenes {};
};
