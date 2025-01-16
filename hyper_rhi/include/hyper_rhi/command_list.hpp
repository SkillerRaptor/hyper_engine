/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <vector>

#include <hyper_core/bit_flags.hpp>

#include "hyper_rhi/forward.hpp"
#include "hyper_rhi/subresource_range.hpp"

namespace hyper_engine
{
    enum class BarrierPipelineStage : uint16_t
    {
        None = 0,
        ComputeShader = 1 << 0,
        VertexShader = 1 << 1,
        FragmentShader = 1 << 2,
        EarlyFragmentTests = 1 << 3,
        LateFragmentTests = 1 << 4,
        ColorAttachmentOutput = 1 << 5,
        Copy = 1 << 6,
        Clear = 1 << 7,
        AllGraphics = 1 << 8,
        AllTransfer = 1 << 9,
        AllCommands = 1 << 10,
    };

    enum class BarrierAccess : uint8_t
    {
        None = 0,
        ShaderRead = 1 << 0,
        ShaderWrite = 1 << 1,
        ColorAttachmentRead = 1 << 2,
        ColorAttachmentWrite = 1 << 3,
        DepthStencilAttachmentRead = 1 << 4,
        DepthStencilAttachmentWrite = 1 << 5,
        TransferRead = 1 << 6,
        TransferWrite = 1 << 7,
    };

    struct MemoryBarrier
    {
        BitFlags<BarrierPipelineStage> stage_before = BarrierPipelineStage::None;
        BitFlags<BarrierPipelineStage> stage_after = BarrierPipelineStage::None;
        BitFlags<BarrierAccess> access_before = BarrierAccess::None;
        BitFlags<BarrierAccess> access_after = BarrierAccess::None;
    };

    struct BufferMemoryBarrier
    {
        BitFlags<BarrierPipelineStage> stage_before = BarrierPipelineStage::None;
        BitFlags<BarrierPipelineStage> stage_after = BarrierPipelineStage::None;
        BitFlags<BarrierAccess> access_before = BarrierAccess::None;
        BitFlags<BarrierAccess> access_after = BarrierAccess::None;
        std::shared_ptr<Buffer> buffer;
    };

    enum class BarrierTextureLayout
    {
        Undefined,
        General,
        Present,
        ColorAttachment,
        DepthStencilAttachment,
        ShaderReadOnly,
        TransferSrc,
        TransferDst,
    };

    struct TextureMemoryBarrier
    {
        BitFlags<BarrierPipelineStage> stage_before = BarrierPipelineStage::None;
        BitFlags<BarrierPipelineStage> stage_after = BarrierPipelineStage::None;
        BitFlags<BarrierAccess> access_before = BarrierAccess::None;
        BitFlags<BarrierAccess> access_after = BarrierAccess::None;
        BarrierTextureLayout layout_before = BarrierTextureLayout::Undefined;
        BarrierTextureLayout layout_after = BarrierTextureLayout::Undefined;
        std::shared_ptr<Texture> texture;
        SubresourceRange subresource_range;
    };

    struct Barriers
    {
        std::vector<MemoryBarrier> memory_barriers;
        std::vector<BufferMemoryBarrier> buffer_memory_barriers;
        std::vector<TextureMemoryBarrier> texture_memory_barriers;
    };

    struct Extent3d
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;
    };

    struct Offset3d
    {
        int32_t x = 0;
        int32_t y = 0;
        int32_t z = 0;
    };

    class CommandList
    {
    public:
        virtual ~CommandList() = default;

        virtual void begin() = 0;
        virtual void end() = 0;

        virtual void insert_barriers(const Barriers &barriers) const = 0;

        virtual void clear_buffer(const std::shared_ptr<Buffer> &buffer, size_t size, uint64_t offset) = 0;
        virtual void clear_texture(const std::shared_ptr<Texture> &texture, SubresourceRange subresource_range) = 0;

        virtual void copy_buffer_to_buffer(
            const std::shared_ptr<Buffer> &src,
            uint64_t src_offset,
            const std::shared_ptr<Buffer> &dst,
            uint64_t dst_offset,
            size_t size) = 0;
        virtual void copy_buffer_to_texture(
            const std::shared_ptr<Buffer> &src,
            uint64_t src_offset,
            const std::shared_ptr<Texture> &dst,
            Offset3d dst_offset,
            Extent3d dst_extent,
            uint32_t dst_mip_level,
            uint32_t dst_array_index) = 0;
        virtual void copy_texture_to_buffer(
            const std::shared_ptr<Texture> &src,
            Offset3d src_offset,
            Extent3d src_extent,
            uint32_t src_mip_level,
            uint32_t src_array_index,
            const std::shared_ptr<Buffer> &dst,
            uint64_t dst_offset) = 0;
        virtual void copy_texture_to_texture(
            const std::shared_ptr<Texture> &src,
            Offset3d src_offset,
            uint32_t src_mip_level,
            uint32_t src_array_index,
            const std::shared_ptr<Texture> &dst,
            Offset3d dst_offset,
            uint32_t dst_mip_level,
            uint32_t dst_array_index,
            Extent3d extent) = 0;

        virtual void write_buffer(const std::shared_ptr<Buffer> &buffer, const void *data, size_t size, uint64_t offset) = 0;
        virtual void write_texture(
            const std::shared_ptr<Texture> &texture,
            Offset3d offset,
            Extent3d extent,
            uint32_t mip_level,
            uint32_t array_index,
            const void *data,
            size_t data_size,
            uint64_t data_offset) = 0;

        std::shared_ptr<ComputePass> begin_compute_pass(const ComputePassDescriptor &descriptor) const;
        std::shared_ptr<RenderPass> begin_render_pass(const RenderPassDescriptor &descriptor) const;

    protected:
        virtual std::shared_ptr<ComputePass> begin_compute_pass_platform(const ComputePassDescriptor &descriptor) const = 0;
        virtual std::shared_ptr<RenderPass> begin_render_pass_platform(const RenderPassDescriptor &descriptor) const = 0;
    };
} // namespace hyper_engine