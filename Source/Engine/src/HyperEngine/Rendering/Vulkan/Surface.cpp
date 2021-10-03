/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Surface.hpp"

#include "HyperEngine/Core/Logger.hpp"

#include <GLFW/glfw3.h>

namespace HyperEngine::Vulkan
{
	auto CSurface::create(const CSurface::SDescription& description) -> bool
	{
		if (description.instance == VK_NULL_HANDLE)
		{
			CLogger::fatal("CSurface::create(): The description vulkan instance is null");
			return false;
		}

		if (description.window == nullptr)
		{
			CLogger::fatal("CSurface::create(): The description window is null");
			return false;
		}

		m_instance = description.instance;

		if (glfwCreateWindowSurface(m_instance, description.window, nullptr, &m_surface) != VK_SUCCESS)
		{ 
			CLogger::fatal("CSurface::create(): Failed to create vulkan window surface");
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

	auto CSurface::surface() const noexcept -> VkSurfaceKHR
	{
		return m_surface;
	}
} // namespace HyperEngine::Vulkan
