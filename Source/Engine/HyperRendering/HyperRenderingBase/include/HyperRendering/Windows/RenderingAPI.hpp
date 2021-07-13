/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
namespace HyperRendering
{
	const char* convert_to_library(RenderingAPI rendering_api)
	{
		switch (rendering_api)
		{
		case RenderingAPI::DirectX11:
			return "HyperDirectX11.dll";
		case RenderingAPI::DirectX12:
			return "HyperDirectX12.dll";
		case RenderingAPI::OpenGL33:
			return "HyperOpenGL33d.dll";
		case RenderingAPI::OpenGL46:
			return "HyperOpenGL46.dll";
		case RenderingAPI::Vulkan:
			return "HyperVulkan.dll";
		default:
			break;
		}

		return "undefined";
	}
} // namespace HyperRendering
#endif
