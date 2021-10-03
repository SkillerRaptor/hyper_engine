/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Prerequisites.hpp"
#include "HyperEngine/Rendering/IContext.hpp"
#include "HyperEngine/Rendering/Vulkan/CommandBuffer.hpp"
#include "HyperEngine/Rendering/Vulkan/Device.hpp"
#include "HyperEngine/Rendering/Vulkan/Surface.hpp"
#include "HyperEngine/Rendering/Vulkan/Swapchain.hpp"

#include <volk.h>

#include <array>
#include <vector>

namespace HyperEngine::Vulkan
{
	class CContext : public IContext
	{
	private:
		static constexpr std::array<const char*, 1> s_validation_layers = { "VK_LAYER_KHRONOS_validation" };

	public:
		~CContext() override;

		auto create(const SDescription& description) -> bool override;

	private:
		auto create_instance() -> bool;
		auto create_debug_messenger() -> bool;

		auto check_validation_layers_support() const -> bool;
		auto request_required_extensions() -> std::vector<const char*>;

	private:
		GLFWwindow* m_window{ nullptr };

		VkInstance m_instance{ VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT m_debug_messenger{ VK_NULL_HANDLE };

		CSurface m_surface{};
		CDevice m_device{};
		CSwapchain m_swapchain{};
		CCommandBuffer m_command_buffer{};

		bool m_validation_layers_enabled{ false };
	};
} // namespace HyperEngine::Vulkan
