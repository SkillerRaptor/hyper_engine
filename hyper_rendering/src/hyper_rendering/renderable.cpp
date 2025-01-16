/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/renderable.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stb_image.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>
#include <hyper_rhi/buffer.hpp>
#include <hyper_rhi/command_list.hpp>
#include <hyper_rhi/graphics_device.hpp>
#include <hyper_rhi/sampler.hpp>
#include <hyper_rhi/shader_compiler.hpp>
#include <hyper_rhi/texture.hpp>
#include <hyper_rhi/texture_view.hpp>

#include "hyper_rendering/mesh.hpp"

#include "shader_interop.h"

namespace hyper_engine
{
    void Node::refresh_transform(const glm::mat4 &parent_matrix)
    {
        world_transform = parent_matrix * local_transform;

        for (const std::shared_ptr<Node> &child : children)
        {
            child->refresh_transform(world_transform);
        }
    }

    void Node::draw(const glm::mat4 &top_matrix, DrawContext &context) const
    {
        for (const std::shared_ptr<Node> &child : children)
        {
            child->draw(top_matrix, context);
        }
    }

    MeshNode::MeshNode(const std::shared_ptr<Mesh> &mesh)
        : mesh(mesh)
    {
    }

    void MeshNode::draw(const glm::mat4 &top_matrix, DrawContext &context) const
    {
        const glm::mat4 node_matrix = top_matrix * world_transform;

        for (const GltfSurface &surface : mesh->surfaces())
        {
            const RenderObject render_object = {
                .index_count = surface.count,
                .first_index = surface.start_index,
                .index_buffer = mesh->indices_buffer(),
                .material = &surface.material->data,
                .transform = node_matrix,
                .mesh_buffer = mesh->mesh_buffer(),
            };

            if (surface.material->data.pass_type == MaterialPassType::MainColor)
            {
                context.opaque_surfaces.push_back(render_object);
            }
            else
            {
                context.transparent_surfaces.push_back(render_object);
            }
        }

        Node::draw(top_matrix, context);
    }

    LoadedGltf::LoadedGltf(
        std::vector<std::shared_ptr<Mesh>> meshes,
        std::vector<std::shared_ptr<Node>> nodes,
        std::vector<std::shared_ptr<Texture>> textures,
        std::vector<std::shared_ptr<TextureView>> texture_views,
        std::vector<std::shared_ptr<GltfMaterial>> materials,
        std::vector<std::shared_ptr<Node>> top_nodes,
        std::vector<std::shared_ptr<Sampler>> samplers)
        : m_meshes(std::move(meshes))
        , m_nodes(std::move(nodes))
        , m_textures(std::move(textures))
        , m_texture_views(std::move(texture_views))
        , m_materials(std::move(materials))
        , m_top_nodes(std::move(top_nodes))
        , m_samplers(std::move(samplers))
    {
    }

    void LoadedGltf::draw(const glm::mat4 &top_matrix, DrawContext &draw_context) const
    {
        for (const std::shared_ptr<Node> &node : m_top_nodes)
        {
            node->draw(top_matrix, draw_context);
        }
    }

    std::shared_ptr<LoadedGltf> load_gltf(
        const std::shared_ptr<CommandList> &command_list,
        const std::shared_ptr<TextureView> &white_texture_view,
        const std::shared_ptr<Texture> &error_texture,
        const std::shared_ptr<TextureView> &error_texture_view,
        const std::shared_ptr<Sampler> &default_sampler_linear,
        const GltfMetallicRoughness &metallic_roughness_material,
        const std::string &path)
    {
        // HE_INFO("Loading GLTF '{}'", path);

        /*
        const std::filesystem::path file_path(path);

        std::string file_name = file_path.filename().generic_string();

        fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath(file_path);
        HE_ASSERT(data.error() == fastgltf::Error::None);

        constexpr fastgltf::Options options = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::LoadExternalBuffers |
                                              fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices;

        fastgltf::Parser parser;
        fastgltf::Expected<fastgltf::Asset> asset = parser.loadGltf(data.get(), file_path.parent_path(), options);
        HE_ASSERT(asset.error() == fastgltf::Error::None);

        std::vector<std::shared_ptr<Sampler>> samplers;
        for (const fastgltf::Sampler &sampler : asset->samplers)
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

            const Filter mag_filter = extract_filter(sampler.magFilter.value_or(fastgltf::Filter::Nearest));
            const Filter min_filter = extract_filter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));
            const Filter mipmap_filter = extract_filter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

            samplers.push_back(
                GraphicsDevice::get()->create_sampler({
                    .label = sampler.name.empty() ? file_name : std::string(sampler.name),
                    .mag_filter = mag_filter,
                    .min_filter = min_filter,
                    .mipmap_filter = mipmap_filter,
                    .address_mode_u = AddressMode::Repeat,
                    .address_mode_v = AddressMode::Repeat,
                    .address_mode_w = AddressMode::Repeat,
                    .mip_lod_bias = 0.0,
                    .compare_operation = CompareOperation::Never,
                    .min_lod = 0.0,
                    .max_lod = 1000.0f,
                    .border_color = BorderColor::TransparentBlack,
                }));
        }

        std::vector<std::shared_ptr<Texture>> textures;
        std::vector<std::shared_ptr<TextureView>> texture_views;
        for (const fastgltf::Image &image : asset->images)
        {
            int32_t width = 0;
            int32_t height = 0;
            int32_t channels = 0;
            uint8_t *image_data = nullptr;

            std::visit(
                fastgltf::visitor{
                    [](auto &)
                    {
                        HE_PANIC();
                    },
                    [&](const fastgltf::sources::URI &image_file_path)
                    {
                        HE_ASSERT(image_file_path.fileByteOffset == 0);
                        HE_ASSERT(image_file_path.uri.isLocalPath());

                        const std::string image_path(image_file_path.uri.path().begin(), image_file_path.uri.path().end());
                        image_data = stbi_load(image_path.c_str(), &width, &height, &channels, 4);
                    },
                    [&](const fastgltf::sources::Array &array)
                    {
                        image_data = stbi_load_from_memory(
                            reinterpret_cast<const unsigned char *>(array.bytes.data()),
                            static_cast<int>(array.bytes.size()),
                            &width,
                            &height,
                            &channels,
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
                                    HE_PANIC();
                                },
                                [&](const fastgltf::sources::Array &array)
                                {
                                    image_data = stbi_load_from_memory(
                                        reinterpret_cast<const unsigned char *>(array.bytes.data() + bufferView.byteOffset),
                                        static_cast<int>(bufferView.byteLength),
                                        &width,
                                        &height,
                                        &channels,
                                        4);
                                },
                            },
                            buffer.data);
                    },
                },
                image.data);

            if (image_data)
            {
                std::shared_ptr<Texture> texture = GraphicsDevice::get()->create_texture({
                    .label = image.name.empty() ? file_name : std::string(image.name),
                    .width = static_cast<uint32_t>(width),
                    .height = static_cast<uint32_t>(height),
                    .depth = 1,
                    .array_size = 1,
                    .mip_levels = 1,
                    .format = Format::Rgba8Srgb,
                    .dimension = Dimension::Texture2D,
                    .usage = TextureUsage::ShaderResource,
                });

                std::shared_ptr<TextureView> texture_view = GraphicsDevice::get()->create_texture_view({
                    .label = image.name.empty() ? file_name : std::string(image.name),
                    .texture = texture,
                    .subresource_range =
                        SubresourceRange{
                            .base_mip_level = 0,
                            .mip_level_count = 1,
                            .base_array_level = 0,
                            .array_layer_count = 1,
                        },
                    .component_mapping =
                        ComponentMapping{
                            .r = ComponentSwizzle::Identity,
                            .g = ComponentSwizzle::Identity,
                            .b = ComponentSwizzle::Identity,
                            .a = ComponentSwizzle::Identity,
                        },
                });

                command_list->insert_barriers({
                    .memory_barriers = {},
                    .buffer_memory_barriers = {},
                    .texture_memory_barriers =
                        {
                            TextureMemoryBarrier{
                                .stage_before = BarrierPipelineStage::AllCommands,
                                .stage_after = BarrierPipelineStage::AllCommands,
                                .access_before = BarrierAccess::None,
                                .access_after = BarrierAccess::TransferWrite,
                                .layout_before = BarrierTextureLayout::Undefined,
                                .layout_after = BarrierTextureLayout::TransferDst,
                                .texture = texture,
                                .subresource_range =
                                    SubresourceRange{
                                        .base_mip_level = 0,
                                        .mip_level_count = 1,
                                        .base_array_level = 0,
                                        .array_layer_count = 1,
                                    },
                            },
                        },
                });

                command_list->write_texture(
                    texture,
                    Offset3d{
                        .x = 0,
                        .y = 0,
                        .z = 0,
                    },
                    Extent3d{
                        .width = static_cast<uint32_t>(width),
                        .height = static_cast<uint32_t>(height),
                        .depth = 1,
                    },
                    0,
                    0,
                    image_data,
                    static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * 4,
                    0);

                stbi_image_free(image_data);

                textures.push_back(texture);
                texture_views.push_back(texture_view);
            }
            else
            {
                textures.push_back(error_texture);
                texture_views.push_back(error_texture_view);
            }
        }

        std::vector<std::shared_ptr<GltfMaterial>> materials;
        for (const fastgltf::Material &material : asset->materials)
        {
            std::shared_ptr<GltfMaterial> new_material = std::make_shared<GltfMaterial>();
            materials.push_back(new_material);

            MaterialPassType pass_type = MaterialPassType::MainColor;
            if (material.alphaMode == fastgltf::AlphaMode::Blend)
            {
                pass_type = MaterialPassType::Transparent;
            }

            GltfMetallicRoughness::MaterialResources material_resources = {
                .color_factors = glm::vec4(
                    material.pbrData.baseColorFactor.x(),
                    material.pbrData.baseColorFactor.y(),
                    material.pbrData.baseColorFactor.z(),
                    material.pbrData.baseColorFactor.w()),
                .color_texture_view = white_texture_view,
                .color_sampler = default_sampler_linear,
                .metal_roughness_factors = glm::vec4(material.pbrData.metallicFactor, material.pbrData.roughnessFactor, 0.0, 0.0),
                .metal_roughness_texture_view = white_texture_view,
                .metal_roughness_sampler = default_sampler_linear,
            };

            if (material.pbrData.baseColorTexture.has_value())
            {
                const size_t image = asset->textures[material.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
                const size_t sampler = asset->textures[material.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

                material_resources.color_texture_view = texture_views[image];
                material_resources.color_sampler = samplers[sampler];
            }

            new_material->data = metallic_roughness_material.write_material(command_list, pass_type, material_resources);
        }

        std::vector<std::shared_ptr<Mesh>> meshes;

        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> normals;
        std::vector<glm::vec4> colors;
        std::vector<glm::vec4> tex_coords;
        std::vector<uint32_t> indices;
        for (const fastgltf::Mesh &mesh : asset->meshes)
        {
            positions.clear();
            normals.clear();
            colors.clear();
            tex_coords.clear();
            indices.clear();

            std::vector<GltfSurface> surfaces;
            for (const fastgltf::Primitive &primitive : mesh.primitives)
            {
                GltfSurface surface = {
                    .start_index = static_cast<uint32_t>(indices.size()),
                    .count = static_cast<uint32_t>(asset->accessors[primitive.indicesAccessor.value()].count),
                    .material = nullptr,
                };

                const auto initial_vertex = static_cast<uint32_t>(positions.size());

                {
                    fastgltf::Accessor &accessor = asset->accessors[primitive.indicesAccessor.value()];

                    fastgltf::iterateAccessor<uint32_t>(
                        asset.get(),
                        accessor,
                        [&](const uint32_t index)
                        {
                            indices.push_back(index + initial_vertex);
                        });
                }

                {
                    fastgltf::Accessor &positions_attribute = asset->accessors[primitive.findAttribute("POSITION")->accessorIndex];
                    positions.resize(positions.size() + positions_attribute.count);
                    normals.resize(normals.size() + positions_attribute.count);
                    colors.resize(colors.size() + positions_attribute.count);
                    tex_coords.resize(tex_coords.size() + positions_attribute.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(
                        asset.get(),
                        positions_attribute,
                        [&](const glm::vec3 value, const size_t index)
                        {
                            positions[initial_vertex + index] = glm::vec4(value.x, value.y, value.z, 1.0);
                            normals[initial_vertex + index] = glm::vec4(1.0, 0.0, 0.0, 0.0);
                            colors[initial_vertex + index] = glm::vec4(1.0, 1.0, 1.0, 0.0);
                            tex_coords[initial_vertex + index] = glm::vec4(0.0, 0.0, 0.0, 0.0);
                        });
                }

                const fastgltf::Attribute *normals_attribute = primitive.findAttribute("NORMAL");
                if (normals_attribute != primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(
                        asset.get(),
                        asset->accessors[normals_attribute->accessorIndex],
                        [&](const glm::vec3 value, const size_t index)
                        {
                            normals[initial_vertex + index] = glm::vec4(value.x, value.y, value.z, 0.0);
                        });
                }

                const fastgltf::Attribute *colors_attribute = primitive.findAttribute("COLOR_0");
                if (colors_attribute != primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(
                        asset.get(),
                        asset->accessors[colors_attribute->accessorIndex],
                        [&](const glm::vec4 value, const size_t index)
                        {
                            colors[initial_vertex + index] = value;
                        });
                }

                const fastgltf::Attribute *tex_coord_attribute = primitive.findAttribute("TEXCOORD_0");
                if (tex_coord_attribute != primitive.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(
                        asset.get(),
                        asset->accessors[tex_coord_attribute->accessorIndex],
                        [&](const glm::vec2 value, const size_t index)
                        {
                            tex_coords[initial_vertex + index] = glm::vec4(value.x, value.y, 0.0, 0.0);
                        });
                }

                if (primitive.materialIndex.has_value())
                {
                    surface.material = materials[primitive.materialIndex.value()];
                }
                else
                {
                    surface.material = materials[0];
                }

                surfaces.push_back(surface);
            }

            const std::shared_ptr<Buffer> positions_buffer = GraphicsDevice::get()->create_buffer({
                .label = fmt::format("{} Positions", mesh.name),
                .byte_size = positions.size() * sizeof(glm::vec4),
                .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
            });
            command_list->write_buffer(positions_buffer, positions.data(), positions.size() * sizeof(glm::vec4), 0);

            const std::shared_ptr<Buffer> normals_buffer = GraphicsDevice::get()->create_buffer({
                .label = fmt::format("{} Normals", mesh.name),
                .byte_size = normals.size() * sizeof(glm::vec4),
                .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
            });
            command_list->write_buffer(normals_buffer, normals.data(), normals.size() * sizeof(glm::vec4), 0);

            const std::shared_ptr<Buffer> colors_buffer = GraphicsDevice::get()->create_buffer({
                .label = fmt::format("{} Colors", mesh.name),
                .byte_size = colors.size() * sizeof(glm::vec4),
                .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
            });
            command_list->write_buffer(colors_buffer, colors.data(), colors.size() * sizeof(glm::vec4), 0);

            const std::shared_ptr<Buffer> tex_coords_buffer = GraphicsDevice::get()->create_buffer({
                .label = fmt::format("{} Tex Coords", mesh.name),
                .byte_size = tex_coords.size() * sizeof(glm::vec4),
                .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
            });
            command_list->write_buffer(tex_coords_buffer, tex_coords.data(), tex_coords.size() * sizeof(glm::vec4), 0);

            const std::shared_ptr<Buffer> mesh_buffer = GraphicsDevice::get()->create_buffer({
                .label = fmt::format("{} Mesh Data", mesh.name),
                .byte_size = sizeof(ShaderMesh),
                .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
            });

            const ShaderMesh shader_mesh = {
                .positions = positions_buffer->handle(),
                .normals = normals_buffer->handle(),
                .colors = colors_buffer->handle(),
                .tex_coords = tex_coords_buffer->handle(),
            };

            command_list->write_buffer(mesh_buffer, &shader_mesh, sizeof(ShaderMesh), 0);

            const std::shared_ptr<Buffer> indices_buffer = GraphicsDevice::get()->create_buffer({
                .label = fmt::format("{} Indices", mesh.name),
                .byte_size = indices.size() * sizeof(uint32_t),
                .usage = BufferUsage::Index,
            });
            command_list->write_buffer(indices_buffer, indices.data(), indices.size() * sizeof(uint32_t), 0);

            auto new_mesh = std::make_shared<Mesh>(
                std::string(mesh.name),
                surfaces,
                positions_buffer,
                normals_buffer,
                colors_buffer,
                tex_coords_buffer,
                mesh_buffer,
                indices_buffer);
            meshes.push_back(new_mesh);
        }

        std::vector<std::shared_ptr<Node>> nodes;
        for (const fastgltf::Node &node : asset->nodes)
        {
            std::shared_ptr<Node> new_node;

            if (node.meshIndex.has_value())
            {
                new_node = std::make_shared<MeshNode>(meshes[node.meshIndex.value()]);
            }
            else
            {
                new_node = std::make_shared<Node>();
            }

            nodes.push_back(new_node);

            std::visit(
                fastgltf::visitor{
                    [&](fastgltf::math::fmat4x4 matrix)
                    {
                        memcpy(&new_node->local_transform, matrix.data(), sizeof(matrix));
                    },
                    [&](fastgltf::TRS transform)
                    {
                        const glm::vec3 translation(transform.translation.x(), transform.translation.y(), transform.translation.z());
                        const glm::quat rotation(transform.rotation.w(), transform.rotation.x(), transform.rotation.y(), transform.rotation.z());
                        const glm::vec3 scale(transform.scale.x(), transform.scale.y(), transform.scale.z());

                        const glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
                        const glm::mat4 rotation_matrix = glm::toMat4(rotation);
                        const glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);

                        new_node->local_transform = translation_matrix * rotation_matrix * scale_matrix;
                    },
                },
                node.transform);
        }

        for (size_t index = 0; index < asset->nodes.size(); ++index)
        {
            fastgltf::Node &node = asset->nodes[index];
            const std::shared_ptr<Node> &scene_node = nodes[index];

            for (const size_t child : node.children)
            {
                scene_node->children.push_back(nodes[child]);
                nodes[child]->parent = scene_node;
            }
        }

        std::vector<std::shared_ptr<Node>> top_nodes;
        for (const std::shared_ptr<Node> &node : nodes)
        {
            if (node->parent.lock() == nullptr)
            {
                top_nodes.push_back(node);
                node->refresh_transform(glm::mat4(1.0));
            }
        }

        return std::make_shared<LoadedGltf>(meshes, nodes, textures, texture_views, materials, top_nodes, samplers);
        */
        return nullptr;
    }
} // namespace hyper_engine