/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#define VK_USE_PLATFORM_WIN32_KHR
#include <HyperCore/Logger.hpp>
#include <HyperRendering/Vulkan/Windows/PlatformContext.hpp>
#include <vulkan/vulkan.h>

namespace HyperRendering::Vulkan::Windows
{
	void CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
		m_window = static_cast<HyperPlatform::Windows::CWindow*>(window);
	}

	void CPlatformContext::shutdown()
	{
	}

	const char* CPlatformContext::get_required_extension() const
	{
		return "VK_KHR_win32_surface";
	}

	bool CPlatformContext::create_window_surface(
		const VkInstance& instance,
		VkSurfaceKHR& surface) const
	{
		VkWin32SurfaceCreateInfoKHR surface_create_info{};
		surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surface_create_info.hwnd = m_window->window();
		surface_create_info.hinstance = m_window->instance();

		if (vkCreateWin32SurfaceKHR(
				instance, &surface_create_info, nullptr, &surface) !=
			VK_SUCCESS)
		{
			HyperCore::CLogger::fatal(
				"Vulkan: failed to create window surface!");
			return false;
		}

		return true;
	}
} // namespace HyperRendering::Vulkan::Windows
