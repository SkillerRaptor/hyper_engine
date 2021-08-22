/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <volk.h>

#include <array>
#include <cstdint>
#include <optional>

namespace HyperRendering::Vulkan
{
	class Context;

	class Device
	{
	public:
		static constexpr std::array<const char*, 1> s_device_extensions = { "VK_KHR_swapchain" };
		
	private:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics_family;
			std::optional<uint32_t> presentation_family;
			
			auto is_complete() const -> bool;
		};

	public:
		explicit Device(Context& context);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto terminate() -> void;
		
		auto find_queue_families(VkPhysicalDevice physical_device) const -> QueueFamilyIndices;
		
		auto physical_device() const -> VkPhysicalDevice;
		auto device() const -> VkDevice;
		
		auto graphics_queue() const -> VkQueue;
		auto presentation_queue() const -> VkQueue;

	private:
		auto pick_physical_device() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto create_device() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;

		auto check_physical_device_extension_support(VkPhysicalDevice physical_device) const -> bool;
		auto is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool;

	private:
		Context& m_context;

		VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
		VkDevice m_device{ VK_NULL_HANDLE };
		
		VkQueue m_graphics_queue{ VK_NULL_HANDLE };
		VkQueue m_presentation_queue{ VK_NULL_HANDLE };
	};
} // namespace HyperRendering::Vulkan
