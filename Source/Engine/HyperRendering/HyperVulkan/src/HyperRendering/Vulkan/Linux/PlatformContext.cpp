/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#define VK_USE_PLATFORM_XLIB_KHR
#include <HyperCore/Logger.hpp>
#include <HyperRendering/Vulkan/Linux/PlatformContext.hpp>
#include <vulkan/vulkan.h>

namespace HyperRendering::Vulkan::Linux
{
	void CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
		m_window = static_cast<HyperPlatform::Linux::CWindow*>(window);
	}

	void CPlatformContext::shutdown()
	{
	}

	const char* CPlatformContext::get_required_extension() const
	{
		return "VK_KHR_xlib_surface";
	}

	bool CPlatformContext::create_window_surface(
		const VkInstance& instance,
		VkSurfaceKHR& surface) const
	{
		VkXlibSurfaceCreateInfoKHR surface_create_info{};
		surface_create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
		surface_create_info.dpy = m_window->display();
		surface_create_info.window = m_window->window();

		if (vkCreateXlibSurfaceKHR(
				instance, &surface_create_info, nullptr, &surface) !=
			VK_SUCCESS)
		{
			HyperCore::CLogger::fatal(
				"Vulkan: failed to create window surface!");
			return false;
		}

		return true;
	}
} // namespace HyperRendering::Vulkan::Linux
