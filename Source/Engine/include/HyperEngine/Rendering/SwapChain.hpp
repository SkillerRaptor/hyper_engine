/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Platform/Window.hpp"
#include "HyperEngine/Rendering/Device.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/OwnPtr.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <vector>
#include <volk.h>

namespace HyperEngine
{
	class SwapChain
	{
	public:
		HYPERENGINE_NON_COPYABLE(SwapChain);

	public:
		SwapChain(const NonNullOwnPtr<Device> &device, const Window &window);

		SwapChain(SwapChain &&other) noexcept;
		SwapChain &operator=(SwapChain &&other) noexcept;

		Expected<void> initialize(const NonNullOwnPtr<VkSurfaceKHR> &surface);

		static Expected<NonNullOwnPtr<SwapChain>> create(
			const NonNullOwnPtr<VkSurfaceKHR> &surface,
			const NonNullOwnPtr<Device> &device,
			const Window &window);

	private:
		VkExtent2D choose_extent(
			const VkSurfaceCapabilitiesKHR &surface_capabilities) const;
		VkSurfaceFormatKHR choose_surface_format(
			const std::vector<VkSurfaceFormatKHR> &surface_formats) const;
		VkPresentModeKHR choose_present_mode(
			const std::vector<VkPresentModeKHR> &present_modes) const;

	private:
		const Device *m_device = nullptr;
		const Window *m_window = nullptr;

		OwnPtr<VkSwapchainKHR> m_swap_chain = nullptr;
		std::vector<NonNullOwnPtr<VkImage>> m_swap_chain_images = {};
		std::vector<VkImageView> m_swap_chain_image_views = {};

		VkFormat m_swap_chain_format = {};
		VkExtent2D m_swap_chain_extent = {};
	};
} // namespace HyperEngine
