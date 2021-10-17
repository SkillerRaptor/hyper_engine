/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Surface.hpp"

#include "HyperEngine/Core/Assertion.hpp"

#include <GLFW/glfw3.h>

namespace HyperEngine::Vulkan
{
	auto CSurface::create(const CSurface::SDescription& description) -> bool
	{
		HYPERENGINE_ASSERT_IS_NOT_NULL(description.window);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.instance, VK_NULL_HANDLE);

		m_instance = description.instance;

		if (glfwCreateWindowSurface(m_instance, description.window, nullptr, &m_surface) != VK_SUCCESS)
		{
			CLogger::fatal("CSurface::create(): Failed to create surface");
			return false;
		}

		return true;
	}

	auto CSurface::destroy() -> void
	{
		if (m_surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		}
	}

	auto CSurface::surface() const noexcept -> const VkSurfaceKHR&
	{
		return m_surface;
	}
} // namespace HyperEngine::Vulkan
