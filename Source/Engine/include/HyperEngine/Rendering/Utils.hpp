/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <volk.h>

namespace HyperEngine::Vulkan
{
	constexpr uint32_t make_version(
		uint32_t major,
		uint32_t minor,
		uint32_t patch) noexcept
	{
		uint32_t value = 0;
		value |= major << 22;
		value |= minor << 12;
		value |= patch << 0;

		return value;
	}

	constexpr uint32_t make_api_version(
		uint32_t variant,
		uint32_t major,
		uint32_t minor,
		uint32_t patch) noexcept
	{
		uint32_t value = 0;
		value |= variant << 29;
		value |= major << 22;
		value |= minor << 12;
		value |= patch << 0;

		return value;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
		VkDebugUtilsMessageTypeFlagsEXT type_flags,
		const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
		void *user_data);
} // namespace HyperEngine::Vulkan
