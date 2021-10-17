/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/RenderPass.hpp"

#include "HyperEngine/Core/Assertion.hpp"
#include "HyperEngine/Rendering/Vulkan/CommandBuffer.hpp"
#include "HyperEngine/Rendering/Vulkan/FrameBuffer.hpp"
#include "HyperEngine/Rendering/Vulkan/Swapchain.hpp"

namespace HyperEngine::Vulkan
{
	auto CRenderPass::create(const CRenderPass::SDescription& description) -> bool
	{
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.device, VK_NULL_HANDLE);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.image_format, VK_FORMAT_MAX_ENUM);

		m_device = description.device;

		VkAttachmentDescription color_attachment_description{};
		color_attachment_description.flags = 0;
		color_attachment_description.format = description.image_format;
		color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_reference{};
		color_attachment_reference.attachment = 0;
		color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass_description{};
		subpass_description.flags = 0;
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.inputAttachmentCount = 0;
		subpass_description.pInputAttachments = nullptr;
		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &color_attachment_reference;
		subpass_description.pResolveAttachments = nullptr;
		subpass_description.pDepthStencilAttachment = nullptr;
		subpass_description.preserveAttachmentCount = 0;
		subpass_description.pPreserveAttachments = nullptr;

		VkRenderPassCreateInfo render_pass_create_info{};
		render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_create_info.pNext = nullptr;
		render_pass_create_info.flags = 0;
		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pAttachments = &color_attachment_description;
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass_description;
		render_pass_create_info.dependencyCount = 0;
		render_pass_create_info.pDependencies = nullptr;

		if (vkCreateRenderPass(m_device, &render_pass_create_info, nullptr, &m_render_pass) != VK_SUCCESS)
		{
			CLogger::fatal("CRenderPass::create(): Failed to create render pass");
			return false;
		}

		return true;
	}

	auto CRenderPass::destroy() -> void
	{
		if (m_render_pass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(m_device, m_render_pass, nullptr);
		}
	}

	auto CRenderPass::begin(
		const CCommandBuffer& command_buffer,
		const CFrameBuffer& frame_buffer,
		const CSwapchain& swapchain,
		const VkClearValue clear_value) const -> void
	{
		VkRenderPassBeginInfo render_pass_begin_info{};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.pNext = nullptr;
		render_pass_begin_info.renderPass = m_render_pass;
		render_pass_begin_info.framebuffer = frame_buffer.frame_buffer();
		render_pass_begin_info.renderArea.offset.x = 0;
		render_pass_begin_info.renderArea.offset.y = 0;
		render_pass_begin_info.renderArea.extent = swapchain.extent();
		render_pass_begin_info.clearValueCount = 1;
		render_pass_begin_info.pClearValues = &clear_value;

		vkCmdBeginRenderPass(command_buffer.command_buffer(), &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	}

	auto CRenderPass::end(const CCommandBuffer& command_buffer) const -> void
	{
		vkCmdEndRenderPass(command_buffer.command_buffer());
	}

	auto CRenderPass::render_pass() const noexcept -> const VkRenderPass&
	{
		return m_render_pass;
	}
} // namespace HyperEngine::Vulkan
