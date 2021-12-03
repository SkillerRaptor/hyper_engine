/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/Forward.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

namespace HyperEngine::Rendering
{
	class Device
	{
	public:
		HYPERENGINE_NON_COPYABLE(Device);

	private:
		struct QueueFamilies
		{
			std::optional<uint32_t> graphics_family = std::nullopt;
		};

	public:
		~Device();

		Device(Device &&other) noexcept;
		Device &operator=(Device &&other) noexcept;

		static Expected<Device *> create(VkInstance instance);

	private:
		Device(VkInstance instance, Error &error);

		Expected<void> pick_physical_device();
		Expected<void> create_device();

		bool is_physical_device_suitable(VkPhysicalDevice physical_device) const;
		QueueFamilies find_queue_families(VkPhysicalDevice physical_device) const;

	private:
		VkInstance m_instance = nullptr;

		VkPhysicalDevice m_physical_device = nullptr;
		VkDevice m_device = nullptr;

		VkQueue m_graphics_queue = nullptr;
	};
} // namespace HyperEngine::Rendering
