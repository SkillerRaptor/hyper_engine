/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <HyperRendering/Vulkan/Linux/PlatformContext.hpp>

namespace HyperRendering::Vulkan::Linux
{
	void CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
	}
	
	void CPlatformContext::shutdown()
	{
	}
	
	const char* CPlatformContext::get_required_extension() const
	{
		return "VK_KHR_xlib_surface";
	}
}
#endif
