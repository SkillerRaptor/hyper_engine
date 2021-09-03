/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <array>

namespace HyperRendering::HyperVulkan
{
	class GraphicsContext;
	
	class Device
	{
	public:
		static constexpr std::array<const char*, 1> s_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		
	public:
		explicit Device(GraphicsContext& t_graphics_context);
		~Device();
		
		auto initialize() -> bool;
		
	private:
		GraphicsContext& m_graphics_context;
		
		VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
		VkDevice m_device{ VK_NULL_HANDLE };
	};
} // namespace HyperRendering::HyperVulkan
