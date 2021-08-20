/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <cstdint>
#include <optional>

using VkPhysicalDevice = struct VkPhysicalDevice_T*;
using VkDevice = struct VkDevice_T*;
using VkQueue = struct VkQueue_T*;

namespace HyperRendering::Vulkan
{
	class Context;

	class Device
	{
	private:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics_family;
			
			auto is_complete() const -> bool;
		};

	public:
		explicit Device(Context& context);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto terminate() -> void;

	private:
		auto pick_physical_device() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto create_device() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;

		auto is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool;
		auto find_queue_families(VkPhysicalDevice physical_device) const -> QueueFamilyIndices;

	private:
		Context& m_context;

		VkPhysicalDevice m_physical_device{ nullptr };
		VkDevice m_device{ nullptr };
		VkQueue m_graphics_queue{ nullptr };
	};
} // namespace HyperRendering::Vulkan
