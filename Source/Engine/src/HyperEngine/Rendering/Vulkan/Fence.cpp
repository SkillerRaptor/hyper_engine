/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Fence.hpp"

#include "HyperEngine/Core/Assertion.hpp"

namespace HyperEngine::Vulkan
{
	auto CFence::create(const CFence::SDescription& description) -> bool
	{
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.device, VK_NULL_HANDLE);

		m_device = description.device;

		VkFenceCreateInfo fence_create_info{};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.pNext = nullptr;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(m_device, &fence_create_info, nullptr, &m_fence) != VK_SUCCESS)
		{
			CLogger::fatal("CFence::create(): Failed to create fence");
			return false;
		}

		return true;
	}

	auto CFence::destroy() -> void
	{
		if (m_device != VK_NULL_HANDLE)
		{
			vkDestroyFence(m_device, m_fence, nullptr);
		}
	}

	auto CFence::wait() const -> bool
	{
		if (vkWaitForFences(m_device, 1, &m_fence, true, 1000000000) != VK_SUCCESS)
		{
			CLogger::fatal("CFence::wait(): Failed to wait for fence");
			return false;
		}

		return true;
	}

	auto CFence::reset() const -> bool
	{
		if (vkResetFences(m_device, 1, &m_fence) != VK_SUCCESS)
		{
			CLogger::fatal("CFence::reset(): Failed to reset fence");
			return false;
		}

		return true;
	}

	auto CFence::fence() const noexcept -> const VkFence&
	{
		return m_fence;
	}
} // namespace HyperEngine::Vulkan
