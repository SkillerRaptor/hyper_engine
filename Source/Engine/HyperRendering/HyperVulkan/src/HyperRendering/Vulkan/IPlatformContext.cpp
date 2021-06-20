/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/PlatformDetection.hpp>
#include <HyperRendering/Vulkan/IPlatformContext.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#	include <HyperRendering/Vulkan/Windows/PlatformContext.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#	include <HyperRendering/Vulkan/Linux/PlatformContext.hpp>
#endif

namespace HyperRendering::Vulkan
{
	IPlatformContext* IPlatformContext::construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new Windows::CPlatformContext();
#elif HYPERENGINE_PLATFORM_LINUX
		return new Linux::CPlatformContext();
#else
		HYPERENGINE_ASSERT_NOT_REACHED();
#endif
	}
} // namespace HyperRendering::Vulkan
