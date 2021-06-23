/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>
#include <HyperRendering/Vulkan/Device.hpp>
#include <array>
#include <vector>

using VkInstance = struct VkInstance_T*;
using VkDebugUtilsMessengerEXT = struct VkDebugUtilsMessengerEXT_T*;

namespace HyperRendering::Vulkan
{
	class IPlatformContext;

	class CContext final : public IContext
	{
	private:
		static constexpr const std::array<const char*, 1>
			s_validation_layers = { "VK_LAYER_KHRONOS_validation" };

	public:
		virtual bool initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;

		virtual void update() override;

		const VkInstance& instance() const;

	private:
		bool create_instance();
		bool setup_debug_messenger();

		static bool is_validation_layer_available();

	private:
		IPlatformContext* m_platform_context{ nullptr };

		bool m_validation_layer_support{ false };
		VkInstance m_instance{ nullptr };
		VkDebugUtilsMessengerEXT m_debug_messenger{ nullptr };
		
		CDevice m_device{};
	};
} // namespace HyperRendering::Vulkan
