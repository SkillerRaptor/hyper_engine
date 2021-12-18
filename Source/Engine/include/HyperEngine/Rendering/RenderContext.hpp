/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <array>
#include <vector>

using VkInstance = struct VkInstance_T *;
using VkDebugUtilsMessengerEXT = struct VkDebugUtilsMessengerEXT_T *;
using VkSurfaceKHR = struct VkSurfaceKHR_T *;

namespace HyperEngine
{
	class Device;
	class SwapChain;
	class Window;

	class RenderContext
	{
	public:
		HYPERENGINE_NON_COPYABLE(RenderContext);

	private:
		static constexpr std::array<const char *, 1> s_validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

	public:
		~RenderContext();

		RenderContext(RenderContext &&other) noexcept;
		RenderContext &operator=(RenderContext &&other) noexcept;

		static Expected<RenderContext *> create(
			bool request_validation_layers,
			const Window &window);

	private:
		explicit RenderContext(
			bool request_validation_layers,
			const Window &window,
			Error &error);

		Expected<void> create_instance();
		Expected<void> create_debug_messenger();

		bool validation_layers_supported() const noexcept;
		std::vector<const char *> get_required_extensions() const;

	private:
		const Window *m_window = nullptr;
		
		VkInstance m_instance = nullptr;
		VkDebugUtilsMessengerEXT m_debug_messenger = nullptr;
		
		VkSurfaceKHR m_surface = nullptr;
		Device *m_device = nullptr;
		SwapChain *m_swap_chain = nullptr;

		bool m_validation_layers_enabled = false;
	};
} // namespace HyperEngine
