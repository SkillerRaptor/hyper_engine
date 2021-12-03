/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

namespace HyperEngine::Rendering
{
	constexpr uint32_t make_version(
		uint32_t major,
		uint32_t minor,
		uint32_t patch) noexcept
	{
		return (major << 22) | (minor << 12) | (patch << 0);
	}

	constexpr uint32_t make_api_version(
		uint32_t variant,
		uint32_t major,
		uint32_t minor,
		uint32_t patch) noexcept
	{
		return (variant << 29) | (major << 22) | (minor << 12) | (patch << 0);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
		VkDebugUtilsMessageTypeFlagsEXT type_flags,
		const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
		void *user_data);
} // namespace HyperEngine::Rendering
