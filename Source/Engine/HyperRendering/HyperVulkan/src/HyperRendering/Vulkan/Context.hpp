/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>
#include <vector>

using VkInstance = struct VkInstance_T*;
using VkDebugUtilsMessengerEXT = struct VkDebugUtilsMessengerEXT_T*;

namespace HyperRendering::Vulkan
{
	class IPlatformContext;
	
	class CContext final : public IContext
	{
	private:
		static const std::vector<const char*> s_validation_layers;
		
	public:
		CContext() = default;
		virtual ~CContext() override = default;
		
		virtual bool initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;
		
		virtual void update() override;
	
	private:
		bool create_instance();
		bool setup_debug_messenger();
		
		static bool is_validation_layer_available();
	
	private:
		IPlatformContext* m_platform_context{ nullptr };
		
		bool m_validation_layer_support{ false };
		VkInstance m_instance{ nullptr };
		VkDebugUtilsMessengerEXT m_debug_messenger{ nullptr };
	};
}
