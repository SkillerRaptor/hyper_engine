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
#include <volk.h>

namespace HyperEngine
{
	class Device
	{
	public:
		HYPERENGINE_NON_COPYABLE(Device);

	public:
		struct QueueFamilies
		{
			std::optional<uint32_t> graphics_family = std::nullopt;
			std::optional<uint32_t> present_family = std::nullopt;
		};

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities = {};
			std::vector<VkSurfaceFormatKHR> formats = {};
			std::vector<VkPresentModeKHR> present_modes = {};
		};

	private:
		static constexpr std::array<const char *, 1> s_device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

	public:
		~Device();

		Device(Device &&other) noexcept;
		Device &operator=(Device &&other) noexcept;

		QueueFamilies find_queue_families(VkPhysicalDevice physical_device) const;
		SwapChainSupportDetails query_swap_chain_support(
			VkPhysicalDevice physical_device) const;

		VkPhysicalDevice physical_device() const noexcept;
		VkDevice device() const noexcept;
		VkQueue graphics_queue() const noexcept;
		VkQueue present_queue() const noexcept;

		static Expected<Device *> create(VkInstance instance, VkSurfaceKHR surface);

	private:
		Device(VkInstance instance, VkSurfaceKHR surface, Error &error);

		Expected<void> find_physical_device();
		Expected<void> create_device();

		bool is_physical_device_suitable(
			VkPhysicalDevice physical_device) const;
		bool check_physical_device_extensions_support(
			VkPhysicalDevice physical_device) const;

	private:
		VkInstance m_instance = nullptr;
		VkSurfaceKHR m_surface = nullptr;

		VkPhysicalDevice m_physical_device = nullptr;
		VkDevice m_device = nullptr;

		VkQueue m_graphics_queue = nullptr;
		VkQueue m_present_queue = nullptr;
	};
} // namespace HyperEngine
