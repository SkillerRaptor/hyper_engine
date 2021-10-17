/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/FrameBuffer.hpp"

#include "HyperEngine/Core/Assertion.hpp"

namespace HyperEngine::Vulkan
{
	auto CFrameBuffer::create(const CFrameBuffer::SDescription& description) -> bool
	{
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.device, VK_NULL_HANDLE);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.image_attachment, VK_NULL_HANDLE);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.render_pass, VK_NULL_HANDLE);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.width, 0);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.height, 0);

		m_device = description.device;

		VkFramebufferCreateInfo framebuffer_create_info{};
		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.pNext = nullptr;
		framebuffer_create_info.flags = 0;
		framebuffer_create_info.renderPass = description.render_pass;
		framebuffer_create_info.attachmentCount = 1;
		framebuffer_create_info.pAttachments = &description.image_attachment;
		framebuffer_create_info.width = description.width;
		framebuffer_create_info.height = description.height;
		framebuffer_create_info.layers = 1;

		if (vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_frame_buffer) != VK_SUCCESS)
		{
			CLogger::fatal("CFrameBuffer::create(): Failed to create frame buffer");
			return false;
		}

		return true;
	}

	auto CFrameBuffer::destroy() -> void
	{
		if (m_frame_buffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(m_device, m_frame_buffer, nullptr);
		}
	}

	auto CFrameBuffer::frame_buffer() const noexcept -> const VkFramebuffer&
	{
		return m_frame_buffer;
	}
} // namespace HyperEngine::Vulkan
