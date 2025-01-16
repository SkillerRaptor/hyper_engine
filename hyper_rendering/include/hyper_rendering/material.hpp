/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glm/glm.hpp>

#include <hyper_core/ref_ptr.hpp>
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
        RefPtr<RenderPipeline> pipeline;
        MaterialPassType pass_type = MaterialPassType::MainColor;

        RefPtr<Buffer> buffer;
    };

    class GltfMetallicRoughness
    {
    public:
        struct MaterialResources
        {
            glm::vec4 color_factors = {0.0f, 0.0f, 0.0f, 0.0f};
            RefPtr<TextureView> color_texture_view;
            RefPtr<Sampler> color_sampler;

            glm::vec4 metal_roughness_factors = {0.0f, 0.0f, 0.0f, 0.0f};
            RefPtr<TextureView> metal_roughness_texture_view;
            RefPtr<Sampler> metal_roughness_sampler;
        };

    public:
        GltfMetallicRoughness(
            GraphicsDevice &graphics_device,
            const ShaderCompiler &shader_compiler,
            const RefPtr<Texture> &render_texture,
            const RefPtr<Texture> &depth_texture);

        MaterialInstance
            write_material(const RefPtr<CommandList> &command_list, MaterialPassType pass_type, const MaterialResources &resources) const;

    private:
        GraphicsDevice &m_graphics_device;

        // FIXME: Make this RefPtr by using a factory function
        RefPtr<RenderPipeline> m_opaque_pipeline;
        RefPtr<RenderPipeline> m_transparent_pipeline;
    };
} // namespace hyper_engine