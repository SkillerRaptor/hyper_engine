/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <HyperRendering/Vulkan/Windows/PlatformContext.hpp>

namespace HyperRendering::Vulkan::Windows
{
	void CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
	}
	
	void CPlatformContext::shutdown()
	{
	}
	
	const char* CPlatformContext::get_required_extension() const
	{
		return "VK_KHR_win32_surface";
	}
}
#endif
