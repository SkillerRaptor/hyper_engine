/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/Forward.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

namespace HyperEngine
{
	class Device
	{
	public:
		HYPERENGINE_NON_COPYABLE(Device);

	private:
		struct QueueFamilies
		{
			std::optional<uint32_t> graphics_family = std::nullopt;
			std::optional<uint32_t> present_family = std::nullopt;
		};

	public:
		~Device();

		Device(Device &&other) noexcept;
		Device &operator=(Device &&other) noexcept;

		static Expected<Device *> create(VkInstance instance, VkSurfaceKHR surface);

	private:
		Device(VkInstance instance, VkSurfaceKHR surface, Error &error);

		Expected<void> find_physical_device();
		Expected<void> create_device();

		bool check_physical_device_suitability(
			VkPhysicalDevice physical_device) const;
		QueueFamilies find_queue_families(VkPhysicalDevice physical_device) const;

	private:
		VkInstance m_instance = nullptr;
		VkSurfaceKHR m_surface = nullptr;

		VkPhysicalDevice m_physical_device = nullptr;
		VkDevice m_device = nullptr;

		VkQueue m_graphics_queue = nullptr;
		VkQueue m_present_queue = nullptr;
	};
} // namespace HyperEngine
