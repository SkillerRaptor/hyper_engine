/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

using VkInstance = struct VkInstance_T*;
using VkSurfaceKHR = struct VkSurfaceKHR_T*;

namespace HyperPlatform
{
	class IWindow;
}

namespace HyperRendering::Vulkan
{
	class IPlatformContext
	{
	public:
		virtual ~IPlatformContext() = default;

		virtual void initialize(HyperPlatform::IWindow* window) = 0;
		virtual void shutdown() = 0;

		virtual const char* get_required_extension() const = 0;
		virtual bool create_window_surface(
			const VkInstance& instance,
			VkSurfaceKHR& surface) const = 0;

		static IPlatformContext* construct();
	};
} // namespace HyperRendering::Vulkan
