/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <volk.h>

namespace HyperRendering::Vulkan
{
	class Context;
	
	class Surface
	{
	public:
		explicit Surface(Context& context);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto terminate() -> void;
		
		auto surface() const -> VkSurfaceKHR;
		
	private:
		Context& m_context;
		
		VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
	};
} // namespace HyperRendering::Vulkan