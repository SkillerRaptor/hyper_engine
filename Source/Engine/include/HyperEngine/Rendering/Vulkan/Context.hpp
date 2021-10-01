/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Prerequisites.hpp"
#include "HyperEngine/Rendering/IContext.hpp"

#include <volk.h>

#include <array>
#include <vector>

namespace HyperEngine::Vulkan
{
	class CContext : public IContext
	{
	public:
		static constexpr std::array<const char*, 1> s_validation_layers = { "VK_LAYER_KHRONOS_validation" };
	
	public:
		~CContext() override;
		
		auto create(const SDescription& description) -> bool override;

	private:
		auto create_instance() -> bool;
		auto create_debug_messenger() -> bool;
		
		auto check_validation_layers_support() const -> bool;
		auto request_required_extensions() -> std::vector<const char*>;
		
		static VKAPI_ATTR auto VKAPI_CALL debug_callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
			VkDebugUtilsMessageTypeFlagsEXT type_flags,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data) -> VkBool32;
		
	private:
		VkInstance m_instance{ VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT m_debug_messenger{ VK_NULL_HANDLE };
		
		bool m_validation_layers_enabled{ false };
	};
} // namespace HyperEngine::Vulkan
