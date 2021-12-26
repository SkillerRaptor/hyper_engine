/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Platform/Window.hpp"
#include "HyperEngine/Rendering/Device.hpp"
#include "HyperEngine/Rendering/SwapChain.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/OwnPtr.hpp"

#include <array>
#include <memory>
#include <vector>

namespace HyperEngine
{
	class RenderContext
	{
	private:
		static constexpr std::array<const char *, 1> s_validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

	public:
		RenderContext() = default;

		RenderContext(RenderContext &&other) noexcept;
		RenderContext &operator=(RenderContext &&other) noexcept;

		Expected<void> initialize(
			bool request_validation_layers,
			const Window &window);

		static Expected<NonNullOwnPtr<RenderContext>> create(
			bool request_validation_layers,
			const Window &window);

	private:
		Expected<void> create_instance();
		Expected<void> create_debug_messenger();

		bool validation_layers_supported() const noexcept;
		std::vector<const char *> get_required_extensions() const;

	private:
		bool m_validation_layers_enabled = false;

		OwnPtr<VkInstance> m_instance = nullptr;
		OwnPtr<VkDebugUtilsMessengerEXT> m_debug_messenger = nullptr;
		OwnPtr<VkSurfaceKHR> m_surface = nullptr;

		OwnPtr<Device> m_device = nullptr;
		OwnPtr<SwapChain> m_swap_chain = nullptr;
	};
} // namespace HyperEngine
