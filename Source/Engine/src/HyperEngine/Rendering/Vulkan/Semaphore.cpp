/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Semaphore.hpp"

#include "HyperEngine/Core/Assertion.hpp"

namespace HyperEngine::Vulkan
{
	auto CSemaphore::create(const CSemaphore::SDescription& description) -> bool
	{
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.device, VK_NULL_HANDLE);

		m_device = description.device;

		VkSemaphoreCreateInfo semaphore_create_info{};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphore_create_info.pNext = nullptr;
		semaphore_create_info.flags = 0;

		if (vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore) != VK_SUCCESS)
		{
			CLogger::fatal("CSemaphore::create(): Failed to create semaphore");
			return false;
		}

		return true;
	}

	auto CSemaphore::destroy() -> void
	{
		if (m_semaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_device, m_semaphore, nullptr);
		}
	}

	auto CSemaphore::semaphore() const noexcept -> const VkSemaphore&
	{
		return m_semaphore;
	}
} // namespace HyperEngine::Vulkan
