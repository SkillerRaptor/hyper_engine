/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <hyper_core/bit_flags.hpp>

#include "hyper_rhi/compare_operation.hpp"
#include "hyper_rhi/format.hpp"
#include "hyper_rhi/forward.hpp"

namespace hyper_engine
{
    enum class BlendFactor : uint8_t
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SrcAlphaSaturate,
        Src1Color,
        OneMinusSrc1Color,
        Src1Alpha,
        OneMinusSrc1Alpha,
    };

    enum class BlendOperation : uint8_t
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
    };

    enum class ColorWrites : uint8_t
    {
        R = 1 << 0,
        G = 1 << 1,
        B = 1 << 2,
        A = 1 << 3,
        Color = R | G | B,
        All = R | G | B | A,
    };

    struct BlendState
    {
        bool blend_enable = false;
        BlendFactor src_blend_factor = BlendFactor::One;
        BlendFactor dst_blend_factor = BlendFactor::Zero;
        BlendOperation operation = BlendOperation::Add;
        BlendFactor alpha_src_blend_factor = BlendFactor::One;
        BlendFactor alpha_dst_blend_factor = BlendFactor::Zero;
        BlendOperation alpha_operation = BlendOperation::Add;
        BitFlags<ColorWrites> color_writes = ColorWrites::All;
    };

    struct ColorAttachmentState
    {
        Format format = Format::Unknown;
        BlendState blend_state = {};
    };

    enum class PrimitiveTopology : uint8_t
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
    };

    enum class FrontFace : uint8_t
    {
        CounterClockwise,
        Clockwise,
    };

    enum class Face : uint8_t
    {
        None,
        Front,
        Back
    };

    enum class PolygonMode : uint8_t
    {
        Fill,
        Line,
        Point,
    };

    struct PrimitiveState
    {
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        FrontFace front_face = FrontFace::CounterClockwise;
        Face cull_mode = Face::None;
        PolygonMode polygon_mode = PolygonMode::Fill;
    };

    struct DepthBiasState
    {
        bool depth_bias_enable = false;
        float constant = 0.0f;
        float clamp = 0.0f;
        float slope = 0.0f;
    };

    struct DepthStencilState
    {
        bool depth_test_enable = false;
        bool depth_write_enable = false;
        Format depth_format = Format::Unknown;
        CompareOperation depth_compare_operation = CompareOperation::Less;
        DepthBiasState depth_bias_state;
    };

    struct RenderPipelineDescriptor
    {
        std::string label;
        std::shared_ptr<PipelineLayout> layout;
        std::shared_ptr<ShaderModule> vertex_shader;
        std::shared_ptr<ShaderModule> fragment_shader;
        std::vector<ColorAttachmentState> color_attachment_states;
        PrimitiveState primitive_state;
        DepthStencilState depth_stencil_state;
    };

    class RenderPipeline
    {
    public:
        virtual ~RenderPipeline() = default;

        std::string_view label() const;
        std::shared_ptr<PipelineLayout> layout() const;
        std::shared_ptr<ShaderModule> vertex_shader() const;
        std::shared_ptr<ShaderModule> fragment_shader() const;
        const std::vector<ColorAttachmentState> &color_attachment_states() const;
        PrimitiveState primitive_state() const;
        DepthStencilState depth_stencil_state() const;

    protected:
        explicit RenderPipeline(const RenderPipelineDescriptor &descriptor);

    protected:
        std::string m_label;
        std::shared_ptr<PipelineLayout> m_layout;
        std::shared_ptr<ShaderModule> m_vertex_shader;
        std::shared_ptr<ShaderModule> m_fragment_shader;
        std::vector<ColorAttachmentState> m_color_attachment_states;
        PrimitiveState m_primitive_state;
        DepthStencilState m_depth_stencil_state;
    };
} // namespace hyper_engine
