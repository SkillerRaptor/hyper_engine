/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "rendering/camera.hpp"
#include "rendering/renderable.hpp"
#include "rhi/graphics_device.hpp"
#include "rhi/surface.hpp"
#include "rhi/shader_compiler.hpp"
#include "rhi/texture.hpp"

#include <memory>
#include <unordered_map>

namespace hyper_engine
{
    class Renderer
    {
    public:
        Renderer(GraphicsDevice &graphics_device, SDL_Window *window);
        ~Renderer();

        void begin_frame(const CameraData &camera);
        void end_frame();
        void present() const;

        void render_scene(const Scene &scene);

    private:
        void create_textures(uint32_t width, uint32_t height);

    private:
        GraphicsDevice &m_graphics_device;

        std::shared_ptr<Surface> m_surface;
        ShaderCompiler m_shader_compiler;
        std::shared_ptr<CommandList> m_command_list;

        std::shared_ptr<Texture> m_render_texture;
        std::shared_ptr<TextureView> m_render_texture_view;
        std::shared_ptr<Texture> m_depth_texture;
        std::shared_ptr<TextureView> m_depth_texture_view;

        std::shared_ptr<Buffer> m_camera_buffer;

        std::shared_ptr<Buffer> m_scene_buffer;

        std::shared_ptr<Texture> m_white_texture;
        std::shared_ptr<TextureView> m_white_texture_view;
        std::shared_ptr<Texture> m_error_texture;
        std::shared_ptr<TextureView> m_error_texture_view;
        std::shared_ptr<Sampler> m_default_sampler_nearest;
        std::shared_ptr<Sampler> m_default_sampler_linear;

        GltfMetallicRoughness m_metallic_roughness_material;

        DrawContext m_draw_context;
        std::unordered_map<std::string, std::shared_ptr<LoadedGltf>> m_scenes;

        std::unique_ptr<OpaquePass> m_opaque_pass;
        std::unique_ptr<GridPass> m_grid_pass;

        uint32_t m_frame_index = 1;
    };
} // namespace hyper_engine