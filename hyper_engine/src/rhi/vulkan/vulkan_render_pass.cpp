/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_render_pass.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/vulkan_buffer.hpp"
#include "hyper_rhi/vulkan/vulkan_descriptor_manager.hpp"
#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"
#include "hyper_rhi/vulkan/vulkan_pipeline_layout.hpp"
#include "hyper_rhi/vulkan/vulkan_render_pipeline.hpp"
#include "hyper_rhi/vulkan/vulkan_texture.hpp"
#include "hyper_rhi/vulkan/vulkan_texture_view.hpp"

namespace hyper_engine
{
    VulkanRenderPass::VulkanRenderPass(
        const RenderPassDescriptor &descriptor,
        VulkanGraphicsDevice &graphics_device,
        const VkCommandBuffer command_buffer)
        : RenderPass(descriptor)
        , m_graphics_device(graphics_device)
        , m_command_buffer(command_buffer)
    {
        m_graphics_device.begin_marker(m_command_buffer, MarkerType::RenderPass, m_label, m_label_color);

        // FIXME: Should this always use the first image?
        const VkExtent2D render_area_extent = {
            .width = m_color_attachments[0].view->texture()->width(),
            .height = m_color_attachments[0].view->texture()->height(),
        };

        constexpr VkOffset2D render_area_offset = {
            .x = 0,
            .y = 0,
        };

        const VkRect2D render_area = {
            .offset = render_area_offset,
            .extent = render_area_extent,
        };

        constexpr VkClearValue clear_value = {
            .color =
                {
                    .float32 =
                        {
                            0.0f,
                            0.0f,
                            0.0f,
                            1.0f,
                        },
                },
        };

        std::vector<VkRenderingAttachmentInfo> color_attachments = {};
        for (const ColorAttachment &color_attachment : m_color_attachments)
        {
            const VulkanTextureView &color_attachment_view = static_cast<const VulkanTextureView &>(*color_attachment.view);

            const VkRenderingAttachmentInfo color_attachment_info = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .pNext = nullptr,
                .imageView = color_attachment_view.image_view(),
                .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                .resolveMode = VK_RESOLVE_MODE_NONE,
                .resolveImageView = VK_NULL_HANDLE,
                .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .loadOp = VulkanRenderPass::get_attachment_load_operation(color_attachment.operation.load_operation),
                .storeOp = VulkanRenderPass::get_attachment_store_operation(color_attachment.operation.store_operation),
                .clearValue = clear_value,
            };

            color_attachments.push_back(color_attachment_info);
        }

        constexpr VkClearValue depth_clear_value = {
            .depthStencil =
                {
                    .depth = 1.0,
                    .stencil = 0,
                },
        };

        const VkImageView depth_attachment_view = m_depth_stencil_attachment.view == nullptr
                                                      ? VK_NULL_HANDLE
                                                      : static_cast<const VulkanTextureView &>(*m_depth_stencil_attachment.view).image_view();

        const VkRenderingAttachmentInfo depth_attachment_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = depth_attachment_view,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VulkanRenderPass::get_attachment_load_operation(m_depth_stencil_attachment.depth_operation.load_operation),
            .storeOp = VulkanRenderPass::get_attachment_store_operation(m_depth_stencil_attachment.depth_operation.store_operation),
            .clearValue = depth_clear_value,
        };

        const VkRenderingInfo rendering_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderArea = render_area,
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = static_cast<uint32_t>(color_attachments.size()),
            .pColorAttachments = color_attachments.data(),
            .pDepthAttachment = depth_attachment_view == nullptr ? nullptr : &depth_attachment_info,
            .pStencilAttachment = nullptr,
        };

        const VkViewport viewport = {
            .x = 0.0,
            .y = 0,
            .width = static_cast<float>(render_area_extent.width),
            .height = static_cast<float>(render_area_extent.height),
            .minDepth = 0.0,
            .maxDepth = 1.0,
        };

        constexpr VkOffset2D offset = {
            .x = 0,
            .y = 0,
        };

        const VkRect2D scissor = {
            .offset = offset,
            .extent = render_area_extent,
        };

        vkCmdBeginRendering(m_command_buffer, &rendering_info);
        vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        vkCmdEndRendering(m_command_buffer);

        m_graphics_device.end_marker(m_command_buffer);
    }

    void VulkanRenderPass::set_pipeline(const std::shared_ptr<RenderPipeline> &pipeline)
    {
        m_pipeline = pipeline;

        const VulkanRenderPipeline &vulkan_pipeline = static_cast<const VulkanRenderPipeline &>(*m_pipeline);
        const VulkanPipelineLayout &layout = static_cast<const VulkanPipelineLayout &>(*m_pipeline->layout());

        const VulkanDescriptorManager &descriptor_manager = static_cast<VulkanDescriptorManager &>(m_graphics_device.descriptor_manager());
        const auto &descriptor_sets = descriptor_manager.descriptor_sets();

        vkCmdBindDescriptorSets(
            m_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            layout.pipeline_layout(),
            0,
            static_cast<uint32_t>(descriptor_sets.size()),
            descriptor_sets.data(),
            0,
            nullptr);

        vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_pipeline.pipeline());
    }

    void VulkanRenderPass::set_index_buffer(const std::shared_ptr<Buffer> &buffer) const
    {
        const VulkanBuffer &vulkan_buffer = static_cast<const VulkanBuffer &>(*buffer);

        vkCmdBindIndexBuffer(m_command_buffer, vulkan_buffer.buffer(), 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanRenderPass::set_scissor(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height) const
    {
        const VkOffset2D offset = {
            .x = x,
            .y = y,
        };

        const VkExtent2D extent = {
            .width = width,
            .height = height,
        };

        const VkRect2D scissor = {
            .offset = offset,
            .extent = extent,
        };

        vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);
    }

    void VulkanRenderPass::set_viewport(
        const float x,
        const float y,
        const float width,
        const float height,
        const float min_depth,
        const float max_depth) const
    {
        const VkViewport viewport = {
            .x = x,
            .y = y,
            .width = width,
            .height = height,
            .minDepth = min_depth,
            .maxDepth = max_depth,
        };

        vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);
    }

    void VulkanRenderPass::set_push_constants(const void *data, const size_t data_size) const
    {
        const VulkanPipelineLayout &layout = static_cast<const VulkanPipelineLayout &>(*m_pipeline->layout());

        vkCmdPushConstants(m_command_buffer, layout.pipeline_layout(), VK_SHADER_STAGE_ALL, 0, static_cast<uint32_t>(data_size), data);
    }

    void VulkanRenderPass::draw(
        const uint32_t vertex_count,
        const uint32_t instance_count,
        const uint32_t first_vertex,
        const uint32_t first_instance) const
    {
        vkCmdDraw(m_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    void VulkanRenderPass::draw_indexed(
        const uint32_t index_count,
        const uint32_t instance_count,
        const uint32_t first_index,
        const int32_t vertex_offset,
        const uint32_t first_instance) const
    {
        vkCmdDrawIndexed(m_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    VkCommandBuffer VulkanRenderPass::command_buffer() const
    {
        return m_command_buffer;
    }

    VkAttachmentLoadOp VulkanRenderPass::get_attachment_load_operation(const LoadOperation load_operation)
    {
        switch (load_operation)
        {
        case LoadOperation::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOperation::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOperation::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            HE_UNREACHABLE();
        }
    }

    VkAttachmentStoreOp VulkanRenderPass::get_attachment_store_operation(const StoreOperation store_operation)
    {
        switch (store_operation)
        {
        case StoreOperation::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case StoreOperation::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default:
            HE_UNREACHABLE();
        }
    }
} // namespace hyper_engine