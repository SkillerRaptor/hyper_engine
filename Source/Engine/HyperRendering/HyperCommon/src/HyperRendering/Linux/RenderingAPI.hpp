/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
namespace HyperRendering
{
	const char* convert_to_library(RenderingAPI rendering_api)
	{
		switch (rendering_api)
		{
		case RenderingAPI::OpenGL33:
			return "libHyperOpenGL33.so";
		case RenderingAPI::OpenGL46:
			return "libHyperOpenGL46.so";
		case RenderingAPI::Vulkan:
			return "libHyperVulkan.so";
		default:
			break;
		}
		
		return "undefined";
	}
}
#endif
