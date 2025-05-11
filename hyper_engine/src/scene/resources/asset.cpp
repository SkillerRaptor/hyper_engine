/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "scene/resources/asset.hpp"

#include <filesystem>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stb_image.h>

#include "core/assertion.hpp"
#include "core/logger.hpp"

Asset Asset::load(const std::string_view path)
{
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    const std::filesystem::path file_path(path);
    fastgltf::Expected<fastgltf::GltfDataBuffer> gltf_data_buffer = fastgltf::GltfDataBuffer::FromPath(file_path);
    HE_ASSERT(gltf_data_buffer.error() == fastgltf::Error::None);

    constexpr fastgltf::Options options = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::LoadExternalBuffers |
                                          fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices;

    fastgltf::Parser parser;
    fastgltf::Expected<fastgltf::Asset> asset = parser.loadGltf(gltf_data_buffer.get(), file_path.parent_path(), options);
    HE_ASSERT(asset.error() == fastgltf::Error::None);

    std::vector<Sampler> samplers;
    for (const fastgltf::Sampler &gltf_sampler : asset->samplers)
    {
        const auto extract_filter = [](const fastgltf::Filter filter) -> Filter
        {
            switch (filter)
            {
            case fastgltf::Filter::Nearest:
            case fastgltf::Filter::NearestMipMapNearest:
            case fastgltf::Filter::NearestMipMapLinear:
                return Filter::Nearest;
            case fastgltf::Filter::Linear:
            case fastgltf::Filter::LinearMipMapNearest:
            case fastgltf::Filter::LinearMipMapLinear:
                return Filter::Linear;
            default:
                HE_UNREACHABLE();
            }
        };

        const Filter mag_filter = extract_filter(gltf_sampler.magFilter.value_or(fastgltf::Filter::Nearest));
        const Filter min_filter = extract_filter(gltf_sampler.minFilter.value_or(fastgltf::Filter::Nearest));

        const Sampler sampler = {
            .mag_filter = mag_filter,
            .min_filter = min_filter,
        };

        samplers.push_back(sampler);
    }

    std::vector<Texture> textures;
    for (const fastgltf::Image &gltf_image : asset->images)
    {
        int32_t width = 0;
        int32_t height = 0;
        uint8_t *image_data = nullptr;
        std::visit(
            fastgltf::visitor{
                [](auto &)
                {
                    HE_UNREACHABLE();
                },
                [&](const fastgltf::sources::URI &image_file_path)
                {
                    HE_ASSERT(image_file_path.fileByteOffset == 0);
                    HE_ASSERT(image_file_path.uri.isLocalPath());

                    const std::string image_path(image_file_path.uri.path().begin(), image_file_path.uri.path().end());
                    image_data = stbi_load(image_path.c_str(), &width, &height, nullptr, 4);
                },
                [&](const fastgltf::sources::Array &array)
                {
                    image_data = stbi_load_from_memory(
                        reinterpret_cast<const unsigned char *>(array.bytes.data()),
                        static_cast<int>(array.bytes.size()),
                        &width,
                        &height,
                        nullptr,
                        4);
                },
                [&](const fastgltf::sources::BufferView &view)
                {
                    const fastgltf::BufferView &bufferView = asset->bufferViews[view.bufferViewIndex];
                    const fastgltf::Buffer &buffer = asset->buffers[bufferView.bufferIndex];

                    std::visit(
                        fastgltf::visitor{
                            [](auto &)
                            {
                                HE_UNREACHABLE();
                            },
                            [&](const fastgltf::sources::Array &array)
                            {
                                image_data = stbi_load_from_memory(
                                    reinterpret_cast<const unsigned char *>(array.bytes.data() + bufferView.byteOffset),
                                    static_cast<int>(bufferView.byteLength),
                                    &width,
                                    &height,
                                    nullptr,
                                    4);
                            },
                        },
                        buffer.data);
                },
            },
            gltf_image.data);

        if (image_data == nullptr)
        {
            HE_PANIC();
        }

        std::vector<uint8_t> data(image_data, image_data + width * height * 4);

        const Texture texture = {
            .width = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .channels = 4,
            .data = data,
        };

        textures.push_back(texture);
    }

    std::vector<Material> materials;
    for (const fastgltf::Material &gltf_material : asset->materials)
    {
        const fastgltf::PBRData &pbr_data = gltf_material.pbrData;

        const glm::vec4 base_color_factors = {
            pbr_data.baseColorFactor.x(),
            pbr_data.baseColorFactor.y(),
            pbr_data.baseColorFactor.z(),
            pbr_data.baseColorFactor.w(),
        };

        std::optional<size_t> base_color_texture = std::nullopt;
        std::optional<size_t> base_color_sampler = std::nullopt;

        if (pbr_data.baseColorTexture.has_value())
        {
            const fastgltf::TextureInfo &texture_info = pbr_data.baseColorTexture.value();
            const fastgltf::Texture &texture = asset->textures[texture_info.textureIndex];

            const size_t image_index = texture.imageIndex.value();
            base_color_texture = image_index;

            const size_t sampler_index = texture.samplerIndex.value();
            base_color_sampler = sampler_index;
        }

        const glm::vec2 metallic_roughness_factor = {
            pbr_data.metallicFactor,
            pbr_data.roughnessFactor,
        };

        std::optional<size_t> metallic_roughness_texture = std::nullopt;
        std::optional<size_t> metallic_roughness_sampler = std::nullopt;

        if (pbr_data.metallicRoughnessTexture.has_value())
        {
            const fastgltf::TextureInfo &texture_info = pbr_data.metallicRoughnessTexture.value();
            const fastgltf::Texture &texture = asset->textures[texture_info.textureIndex];

            const size_t image_index = texture.imageIndex.value();
            metallic_roughness_texture = image_index;

            const size_t sampler_index = texture.samplerIndex.value();
            metallic_roughness_sampler = sampler_index;
        }

        std::optional<size_t> normal_texture = std::nullopt;
        std::optional<size_t> normal_sampler = std::nullopt;
        float normal_scale = 1.0f;
        if (gltf_material.normalTexture.has_value())
        {
            const fastgltf::NormalTextureInfo &normal_texture_info = gltf_material.normalTexture.value();
            const fastgltf::Texture &texture = asset->textures[normal_texture_info.textureIndex];

            const size_t image_index = texture.imageIndex.value();
            normal_texture = image_index;

            const size_t sampler_index = texture.samplerIndex.value();
            normal_sampler = sampler_index;

            normal_scale = normal_texture_info.scale;
        }

        const AlphaMode alpha_mode = [&gltf_material]()
        {
            switch (gltf_material.alphaMode)
            {
            case fastgltf::AlphaMode::Opaque:
            default:
                return AlphaMode::Opaque;
            case fastgltf::AlphaMode::Blend:
                return AlphaMode::Transparent;
            }
        }();

        const Material material = {
            .color_factors = base_color_factors,
            .base_color_texture_index = base_color_texture,
            .base_color_sampler_index = base_color_sampler,
            .metallic_roughness_factor = metallic_roughness_factor,
            .metallic_roughness_texture_index = metallic_roughness_texture,
            .metallic_roughness_sampler_index = metallic_roughness_sampler,
            .normal_texture_index = normal_texture,
            .normal_sampler_index = normal_sampler,
            .normal_scale = normal_scale,
            .alpha_mode = alpha_mode,
            .alpha_cutoff = gltf_material.alphaCutoff,
        };

        materials.push_back(material);
    }

    // NOTE: The gltf meshes are models and gltf primitives are meshes
    std::vector<Model> models;
    for (const fastgltf::Mesh &gltf_mesh : asset->meshes)
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;
        std::vector<glm::vec2> uvs;
        std::vector<uint32_t> indices;
        std::vector<Mesh> meshes;
        for (const fastgltf::Primitive &gltf_primitive : gltf_mesh.primitives)
        {
            const uint32_t start_index = static_cast<uint32_t>(indices.size());
            const auto initial_vertex = static_cast<uint32_t>(positions.size());

            fastgltf::Accessor &accessor = asset->accessors[gltf_primitive.indicesAccessor.value()];

            fastgltf::iterateAccessor<uint32_t>(
                asset.get(),
                accessor,
                [&](const uint32_t index)
                {
                    indices.push_back(index + initial_vertex);
                });

            fastgltf::Accessor &positions_attribute = asset->accessors[gltf_primitive.findAttribute("POSITION")->accessorIndex];
            positions.resize(positions.size() + positions_attribute.count);
            normals.resize(normals.size() + positions_attribute.count);
            colors.resize(colors.size() + positions_attribute.count);
            uvs.resize(uvs.size() + positions_attribute.count);

            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                asset.get(),
                positions_attribute,
                [&](const glm::vec3 value, const size_t index)
                {
                    positions[initial_vertex + index] = value;
                    normals[initial_vertex + index] = glm::vec3(1.0, 0.0, 0.0);
                    colors[initial_vertex + index] = glm::vec3(1.0, 1.0, 1.0);
                    uvs[initial_vertex + index] = glm::vec2(0.0, 0.0);
                });

            const fastgltf::Attribute *normals_attribute = gltf_primitive.findAttribute("NORMAL");
            if (normals_attribute != gltf_primitive.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    asset.get(),
                    asset->accessors[normals_attribute->accessorIndex],
                    [&](const glm::vec3 value, const size_t index)
                    {
                        normals[initial_vertex + index] = value;
                    });
            }

            const fastgltf::Attribute *colors_attribute = gltf_primitive.findAttribute("COLOR_0");
            if (colors_attribute != gltf_primitive.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(
                    asset.get(),
                    asset->accessors[colors_attribute->accessorIndex],
                    [&](const glm::vec4 value, const size_t index)
                    {
                        colors[initial_vertex + index] = value;
                    });
            }

            const fastgltf::Attribute *uvs_attribute = gltf_primitive.findAttribute("TEXCOORD_0");
            if (uvs_attribute != gltf_primitive.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                    asset.get(),
                    asset->accessors[uvs_attribute->accessorIndex],
                    [&](const glm::vec2 value, const size_t index)
                    {
                        uvs[initial_vertex + index] = value;
                    });
            }

            const uint32_t index_count = static_cast<uint32_t>(asset->accessors[gltf_primitive.indicesAccessor.value()].count);
            const size_t material_index = gltf_primitive.materialIndex.value_or(0);
            const Mesh mesh = {
                .start_index = start_index,
                .index_count = index_count,
                .material_index = material_index,
            };

            meshes.push_back(mesh);
        }

        const Model model = {
            .positions = positions,
            .normals = normals,
            .colors = colors,
            .uvs = uvs,
            .indices = indices,
            .meshes = meshes,
        };

        models.push_back(model);
    }

    std::vector<std::unique_ptr<Node>> nodes;
    for (const fastgltf::Node &gltf_node : asset->nodes)
    {
        glm::mat4 local_transform = glm::mat4(1.0);

        std::visit(
            fastgltf::visitor{
                [&](fastgltf::math::fmat4x4 matrix)
                {
                    memcpy(&local_transform, matrix.data(), sizeof(matrix));
                },
                [&](fastgltf::TRS transform)
                {
                    const glm::vec3 translation(transform.translation.x(), transform.translation.y(), transform.translation.z());
                    const glm::quat rotation(transform.rotation.w(), transform.rotation.x(), transform.rotation.y(), transform.rotation.z());
                    const glm::vec3 scale(transform.scale.x(), transform.scale.y(), transform.scale.z());

                    const glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
                    const glm::mat4 rotation_matrix = glm::toMat4(rotation);
                    const glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);

                    local_transform = translation_matrix * rotation_matrix * scale_matrix;
                },
            },
            gltf_node.transform);

        std::optional<size_t> model_index = std::nullopt;
        if (gltf_node.meshIndex.has_value())
        {
            model_index = gltf_node.meshIndex.value();
        }

        std::unique_ptr<Node> node = std::make_unique<Node>(nullptr, std::vector<Node *>{}, local_transform, model_index);
        nodes.push_back(std::move(node));
    }

    for (size_t i = 0; i < asset->nodes.size(); ++i)
    {
        std::unique_ptr<Node> &node = nodes[i];

        const fastgltf::Node &gltf_node = asset->nodes[i];
        for (const size_t child : gltf_node.children)
        {
            node->children.push_back(nodes[child].get());
            nodes[child]->parent = node.get();
        }
    }

    std::vector<Scene> scenes;
    for (const fastgltf::Scene &gltf_scene : asset->scenes)
    {
        std::vector<size_t> node_indices;
        for (const size_t node_index : gltf_scene.nodeIndices)
        {
            node_indices.push_back(node_index);
        }

        const Scene scene = {
            .node_indices = node_indices,
        };
        scenes.push_back(scene);
    }

    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    HE_INFO("Loaded '{}' in {:.2}s", path, elapsed_seconds.count());

    return Asset{
        std::move(samplers),
        std::move(textures),
        std::move(materials),
        std::move(models),
        std::move(nodes),
        std::move(scenes),
    };
}

const std::vector<Asset::Sampler> &Asset::samplers() const
{
    return m_samplers;
}

const std::vector<Asset::Texture> &Asset::textures() const
{
    return m_textures;
}

const std::vector<Asset::Material> &Asset::materials() const
{
    return m_materials;
}

const std::vector<Asset::Model> &Asset::models() const
{
    return m_models;
}

const std::vector<std::unique_ptr<Asset::Node>> &Asset::nodes() const
{
    return m_nodes;
}

const std::vector<Asset::Scene> &Asset::scenes() const
{
    return m_scenes;
}

Asset::Asset(
    std::vector<Sampler> &&samplers,
    std::vector<Texture> &&textures,
    std::vector<Material> &&materials,
    std::vector<Model> &&models,
    std::vector<std::unique_ptr<Node>> &&nodes,
    std::vector<Scene> &&scenes)
    : m_samplers(std::move(samplers))
    , m_textures(std::move(textures))
    , m_materials(std::move(materials))
    , m_models(std::move(models))
    , m_nodes(std::move(nodes))
    , m_scenes(std::move(scenes))
{
}