/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include <hyper_core/key.hpp>
#include <hyper_core/memory.hpp>

#include "hyper_rhi/definitions.hpp"
#include "hyper_rhi/forward.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/shader.hpp"

namespace he
{
    struct Operations
    {
        LoadOperation load_op = LoadOperation::Clear;
        StoreOperation store_op = StoreOperation::Store;
    };

    struct PrimitiveState
    {
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        FrontFace front_face = FrontFace::CounterClockwise;
        Face cull_mode = Face::None;
        PolygonMode polygon_mode = PolygonMode::Fill;
    };

    struct MultisampleState
    {
        u32 sample_count = 1;
    };

    struct BlendState
    {
        bool enable = false;
        BlendFactor src_factor = BlendFactor::One;
        BlendFactor dst_factor = BlendFactor::Zero;
        BlendOperation operation = BlendOperation::Add;
        BlendFactor alpha_src_factor = BlendFactor::One;
        BlendFactor alpha_dst_factor = BlendFactor::Zero;
        BlendOperation alpha_operation = BlendOperation::Add;
        ColorWrites color_writes = ColorWrites::All;
    };

    struct ColorAttachmentState
    {
        Format format = Format::None;
        BlendState blend_state;
    };

    struct DepthBiasState
    {
        bool enable = false;
        float constant = 0.0f;
        float clamp = 0.0f;
        float slope = 0.0f;
    };

    struct DepthStencilState
    {
        bool depth_test_enable = false;
        bool depth_write_enable = false;
        Format depth_format = Format::None;
        CompareOperation depth_compare_operation = CompareOperation::Less;
        DepthBiasState depth_bias_state;
    };

    struct RenderPipelineDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        PipelineLayout layout;
        Shader vertex_shader;
        Shader fragment_shader;
        PrimitiveState primitive_state;
        MultisampleState multisample_state;
        std::vector<ColorAttachmentState> color_attachment_states;
        std::optional<DepthStencilState> depth_stencil_state = std::nullopt;
    };

    class RenderPipeline
    {
    public:
        RenderPipeline(Key<GraphicsDevice>, RefPtr<void> internal_state, const RenderPipelineDescriptor &desc)
            : m_internal_state(std::move(internal_state))
            , m_layout(desc.layout)
            , m_vertex_shader(desc.vertex_shader)
            , m_fragment_shader(desc.fragment_shader)
            , m_primitive_state(desc.primitive_state)
            , m_multisample_state(desc.multisample_state)
            , m_color_attachment_states(desc.color_attachment_states)
            , m_depth_stencil_state(desc.depth_stencil_state)
        {
        }

        HE_ALWAYS_INLINE void *internal_state(Key<GraphicsDevice>) const { return m_internal_state.get(); }

        HE_ALWAYS_INLINE PipelineLayout layout() const { return m_layout; }
        HE_ALWAYS_INLINE Shader vertex_shader() const { return m_vertex_shader; }
        HE_ALWAYS_INLINE Shader fragment_shader() const { return m_fragment_shader; }
        HE_ALWAYS_INLINE PrimitiveState primitive_state() const { return m_primitive_state; }
        HE_ALWAYS_INLINE MultisampleState multisample_state() const { return m_multisample_state; }
        HE_ALWAYS_INLINE std::span<const ColorAttachmentState> color_attachment_states() const
        {
            return m_color_attachment_states;
        }
        HE_ALWAYS_INLINE std::optional<DepthStencilState> depth_stencil_state() const { return m_depth_stencil_state; }

    private:
        RefPtr<void> m_internal_state = nullptr;

        PipelineLayout m_layout;
        Shader m_vertex_shader;
        Shader m_fragment_shader;
        PrimitiveState m_primitive_state;
        MultisampleState m_multisample_state;
        std::vector<ColorAttachmentState> m_color_attachment_states;
        std::optional<DepthStencilState> m_depth_stencil_state = std::nullopt;
    };
} // namespace he
