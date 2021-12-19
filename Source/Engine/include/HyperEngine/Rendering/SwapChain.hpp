/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <vector>
#include <volk.h>

namespace HyperEngine
{
	class Device;
	class Window;

	class SwapChain
	{
	public:
		HYPERENGINE_NON_COPYABLE(SwapChain);

	public:
		~SwapChain();

		SwapChain(SwapChain &&other) noexcept;
		SwapChain &operator=(SwapChain &&other) noexcept;

		static Expected<SwapChain *> create(
			VkSurfaceKHR surface,
			const Device &device,
			const Window &window);

	private:
		SwapChain(
			VkSurfaceKHR surface,
			const Device &device,
			const Window &window,
			Error &error);

		VkExtent2D choose_extent(
			const VkSurfaceCapabilitiesKHR &surface_capabilities) const;
		VkSurfaceFormatKHR choose_surface_format(
			const std::vector<VkSurfaceFormatKHR> &surface_formats) const;
		VkPresentModeKHR choose_present_mode(
			const std::vector<VkPresentModeKHR> &present_modes) const;

	private:
		const Device *m_device = nullptr;
		const Window *m_window = nullptr;

		VkSwapchainKHR m_swap_chain = nullptr;
		VkFormat m_swap_chain_format = {};
		VkExtent2D m_swap_chain_extent = {};
		std::vector<VkImage> m_swap_chain_images = {};
		std::vector<VkImageView> m_swap_chain_image_views = {};
	};
} // namespace HyperEngine
