/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#define HYPERENGINE_SHARED_EXPORT
#include <HyperPlatform/SharedLibrary.hpp>

namespace HyperRendering
{
	enum class RenderingAPI
	{
		DirectX11,
		DirectX12,
		OpenGL33,
		OpenGL46,
		Vulkan
	};

	extern "C" HYPERENGINE_API const char* convert_to_library(RenderingAPI rendering_api);
} // namespace HyperRendering

#if HYPERENGINE_PLATFORM_WINDOWS
#	include <HyperRendering/Windows/RenderingAPI.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#	include <HyperRendering/Linux/RenderingAPI.hpp>
#endif
