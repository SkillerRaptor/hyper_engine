/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperVulkan/Device.hpp"

#include <HyperRendering/IContext.hpp>

#include <vector>

using VkInstance = struct VkInstance_T*;
using VkDebugUtilsMessengerEXT = struct VkDebugUtilsMessengerEXT_T*;

namespace HyperRendering::Vulkan
{
	class Context final : public IContext
	{
	private:
		static constexpr std::array<const char*, 1> s_validation_layers = { "VK_LAYER_KHRONOS_validation" };

	public:
		explicit Context(HyperPlatform::Window& window);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError> override;
		auto terminate() -> void override;
		auto update() -> void override;
		
		auto instance() const -> VkInstance;
		
		static auto validation_layers() -> std::array<const char*, 1>;

	private:
		auto create_instance() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto create_debug_messenger() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		
		auto get_required_extensions() const -> std::vector<const char*>;
		auto are_validation_layers_supported() const -> bool;

	private:
		VkInstance m_instance{ nullptr };
		VkDebugUtilsMessengerEXT m_debug_messenger{ nullptr };
		
		Device m_device;
	};
} // namespace HyperRendering::Vulkan
