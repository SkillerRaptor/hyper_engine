/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <hyper_rhi/forward.hpp>
#include <hyper_rhi/render_pipeline.hpp>

namespace hyper_engine
{
    enum class MaterialPassType : uint8_t
    {
        MainColor,
        Transparent,
    };

    struct MaterialInstance
    {
        std::shared_ptr<RenderPipeline> pipeline;
        MaterialPassType pass_type = MaterialPassType::MainColor;

        std::shared_ptr<Buffer> buffer;
    };

    class GltfMetallicRoughness
    {
    public:
        struct MaterialResources
        {
            glm::vec4 color_factors = {0.0f, 0.0f, 0.0f, 0.0f};
            std::shared_ptr<TextureView> color_texture_view;
            std::shared_ptr<Sampler> color_sampler;

            glm::vec4 metal_roughness_factors = {0.0f, 0.0f, 0.0f, 0.0f};
            std::shared_ptr<TextureView> metal_roughness_texture_view;
            std::shared_ptr<Sampler> metal_roughness_sampler;
        };

    public:
        GltfMetallicRoughness(
            GraphicsDevice &graphics_device,
            const ShaderCompiler &shader_compiler,
            const std::shared_ptr<Texture> &render_texture,
            const std::shared_ptr<Texture> &depth_texture);

        MaterialInstance
            write_material(const std::shared_ptr<CommandList> &command_list, MaterialPassType pass_type, const MaterialResources &resources) const;

    private:
        GraphicsDevice &m_graphics_device;

        // FIXME: Make this std::shared_ptr by using a factory function
        std::shared_ptr<RenderPipeline> m_opaque_pipeline;
        std::shared_ptr<RenderPipeline> m_transparent_pipeline;
    };
} // namespace hyper_engine