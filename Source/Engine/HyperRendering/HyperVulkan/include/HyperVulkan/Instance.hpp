/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <volk.h>

#include <array>
#include <vector>

namespace HyperRendering::Vulkan
{
	class Context;

	class Instance
	{
	public:
		static constexpr std::array<const char*, 1> s_validation_layers = { "VK_LAYER_KHRONOS_validation" };

	public:
		explicit Instance(Context& context);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto terminate() -> void;
		
		auto instance() const -> VkInstance;
		auto debug_messenger() const -> VkDebugUtilsMessengerEXT;
		
	private:
		auto create_instance() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto create_debug_messenger() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;

		auto get_required_extensions() const -> std::vector<const char*>;
		auto are_validation_layers_supported() const -> bool;

		static VKAPI_ATTR auto VKAPI_CALL debug_callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
			VkDebugUtilsMessageTypeFlagsEXT type_flags,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data) -> VkBool32;

	private:
		Context& m_context;

		VkInstance m_instance{ VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT m_debug_messenger{ VK_NULL_HANDLE };
	};
} // namespace HyperRendering::Vulkan
