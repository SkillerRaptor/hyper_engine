/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/FrameBuffer.hpp"

#include "HyperEngine/Core/Logger.hpp"

namespace HyperEngine::Vulkan
{
	auto CFrameBuffer::create(const CFrameBuffer::SDescription& description) -> bool
	{
		if (description.device == VK_NULL_HANDLE)
		{
			CLogger::fatal("CFrameBuffer::create(): The description vulkan device is null");
			return false;
		}

		if (description.render_pass == VK_NULL_HANDLE)
		{
			CLogger::fatal("CFrameBuffer::create(): The description vulkan render pass is null");
			return false;
		}

		if (description.swapchain_image_view == VK_NULL_HANDLE)
		{
			CLogger::fatal("CFrameBuffer::create(): The description vulkan swapchain image view is null");
			return false;
		}

		m_device = description.device;

		VkFramebufferCreateInfo framebuffer_create_info{};
		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.pNext = nullptr;
		framebuffer_create_info.flags = 0;
		framebuffer_create_info.renderPass = description.render_pass;
		framebuffer_create_info.attachmentCount = 1;
		framebuffer_create_info.pAttachments = &description.swapchain_image_view;
		framebuffer_create_info.width = description.swapchain_extent.width;
		framebuffer_create_info.height = description.swapchain_extent.height;
		framebuffer_create_info.layers = 1;

		if (vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_framebuffer) != VK_SUCCESS)
		{
			CLogger::fatal("CFrameBuffer::create(): The description vulkan swapchain image view is null");
			return false;
		}

		return true;
	}

	auto CFrameBuffer::destroy() -> void
	{
		if (m_framebuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
		}
	}
} // namespace HyperEngine::Vulkan
