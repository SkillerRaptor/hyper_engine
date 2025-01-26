/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "hyper_rhi/label_color.hpp"
#include "hyper_rhi/forward.hpp"

namespace hyper_engine
{
    enum class LoadOperation : uint8_t
    {
        Clear,
        Load,
        DontCare,
    };

    enum class StoreOperation : uint8_t
    {
        Store,
        DontCare,
    };

    struct Operations
    {
        LoadOperation load_operation = LoadOperation::Clear;
        StoreOperation store_operation = StoreOperation::Store;
    };

    struct ColorAttachment
    {
        std::shared_ptr<TextureView> view;
        Operations operation;
    };

    struct DepthStencilAttachment
    {
        std::shared_ptr<TextureView> view;
        Operations depth_operation;
        // FIXME: Add stencil operation
    };

    struct RenderPassDescriptor
    {
        std::string label;
        LabelColor label_color;
        std::vector<ColorAttachment> color_attachments;
        DepthStencilAttachment depth_stencil_attachment;
    };

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;

        // FIXME: This should be std::shared_ptr
        virtual void set_pipeline(const std::shared_ptr<RenderPipeline> &pipeline) = 0;
        virtual void set_push_constants(const void *data, size_t data_size) const = 0;

        virtual void set_index_buffer(const std::shared_ptr<Buffer> &buffer) const = 0;

        virtual void set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height) const = 0;
        virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) const = 0;

        virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) const = 0;
        virtual void
            draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
                const = 0;

        // FIXME: Add indirect

        std::string_view label() const;
        LabelColor label_color() const;
        const std::vector<ColorAttachment> &color_attachments() const;
        DepthStencilAttachment depth_stencil_attachment() const;

    protected:
        explicit RenderPass(const RenderPassDescriptor &descriptor);

    protected:
        std::string m_label;
        LabelColor m_label_color;
        std::vector<ColorAttachment> m_color_attachments;
        DepthStencilAttachment m_depth_stencil_attachment;
    };
} // namespace hyper_engine