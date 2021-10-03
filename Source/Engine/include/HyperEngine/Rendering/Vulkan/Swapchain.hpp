/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/Vulkan/Device.hpp"

#include <volk.h>

#include <vector>

struct GLFWwindow;

namespace HyperEngine::Vulkan
{
	class CSwapchain
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };
			VkSurfaceKHR surface{ VK_NULL_HANDLE };

			GLFWwindow* window{ nullptr };

			CDevice::SSwapchainSupportDetails swapchain_support_details{};
			CDevice::SQueueFamilies queue_families{};
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		[[nodiscard]] auto image_format() const noexcept -> VkFormat;
		[[nodiscard]] auto extent() const noexcept -> VkExtent2D;

	private:
		auto create_swapchain() -> bool;
		auto create_image_views() -> bool;

		[[nodiscard]] auto choose_extent(const VkSurfaceCapabilitiesKHR& surface_capabilities) const -> VkExtent2D;
		[[nodiscard]] auto choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_surface_formats) const -> VkSurfaceFormatKHR;
		[[nodiscard]] auto choose_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) const -> VkPresentModeKHR;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };
		VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
		GLFWwindow* m_window{ nullptr };

		CDevice::SSwapchainSupportDetails m_swapchain_support_details{};
		CDevice::SQueueFamilies m_queue_families{};

		VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };
		VkFormat m_image_format{};
		VkExtent2D m_extent{};

		std::vector<VkImage> m_images{};
		std::vector<VkImageView> m_image_views{};
	};
} // namespace HyperEngine::Vulkan
