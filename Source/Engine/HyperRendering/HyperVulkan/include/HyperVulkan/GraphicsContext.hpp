/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRenderingBase/IGraphicsContext.hpp>

#include <volk.h>

#include <array>

namespace HyperRendering::HyperVulkan
{
	class GraphicsContext : public IGraphicsContext
	{
	public:
		static constexpr std::array<const char*, 1> s_validation_layers = { "VK_LAYER_KHRONOS_validation" };

	public:
		GraphicsContext(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window);
		~GraphicsContext() override;
		
		auto initialize() -> HyperCore::InitializeResult override;

		auto update() -> void override;

	private:
		auto create_instance() -> HyperCore::InitializeResult;
		auto create_debug_messenger() -> HyperCore::InitializeResult;

		auto validation_layers_supported() const -> bool;
		auto get_required_extensions() const -> std::vector<const char*>;
		
		static VKAPI_ATTR auto VKAPI_CALL debug_callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
			VkDebugUtilsMessageTypeFlagsEXT type_flags,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data) -> VkBool32;
		
	private:
		VkInstance m_instance{ VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT m_debug_messenger{ VK_NULL_HANDLE };

#if HYPERENGINE_DEBUG
		bool m_validation_layers_enabled{ true };
#else
		bool m_validation_layers_enabled{ false };
#endif
	};
} // namespace HyperRendering::HyperVulkan
