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
	class CSemaphore;

	class CSwapchain
	{
	public:
		struct SDescription
		{
			GLFWwindow* window{ nullptr };

			VkDevice device{ VK_NULL_HANDLE };
			VkSurfaceKHR surface{ VK_NULL_HANDLE };

			CDevice::SSwapchainSupportDetails swapchain_support_details{};
			CDevice::SQueueFamilies queue_families{};

			uint32_t window_width{ 0 };
			uint32_t window_height{ 0 };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		[[nodiscard]] auto acquire_next_image(const CSemaphore& present_semaphore, uint32_t& image_index) const -> bool;

		[[nodiscard]] auto images() const -> std::vector<VkImage>;
		[[nodiscard]] auto image_views() const -> std::vector<VkImageView>;

		[[nodiscard]] auto extent() const noexcept -> VkExtent2D;
		[[nodiscard]] auto image_format() const noexcept -> VkFormat;

		[[nodiscard]] auto swapchain() const noexcept -> const VkSwapchainKHR&;

	private:
		auto create_swapchain(
			VkSurfaceKHR surface,
			const CDevice::SSwapchainSupportDetails& swapchain_support_details,
			const CDevice::SQueueFamilies& queue_families,
			const uint32_t window_width,
			const uint32_t window_height) -> bool;
		auto create_image_views() -> bool;

		[[nodiscard]] auto choose_extent(
			const VkSurfaceCapabilitiesKHR& surface_capabilities,
			const uint32_t width,
			const uint32_t height) const -> VkExtent2D;
		[[nodiscard]] auto choose_surface_format(
			const std::vector<VkSurfaceFormatKHR>& available_surface_formats) const -> VkSurfaceFormatKHR;
		[[nodiscard]] auto choose_present_mode(
			const std::vector<VkPresentModeKHR>& available_present_modes) const -> VkPresentModeKHR;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		std::vector<VkImage> m_images{};
		std::vector<VkImageView> m_image_views{};
		VkExtent2D m_extent{};
		VkFormat m_image_format{};

		VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
