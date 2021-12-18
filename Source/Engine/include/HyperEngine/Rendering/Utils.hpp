/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

namespace HyperEngine::Utils
{
	VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
		void *);

	constexpr uint32_t make_api_version(
		uint32_t variant,
		uint32_t major,
		uint32_t minor,
		uint32_t patch) noexcept
	{
		return (variant << 29) | (major << 22) | (minor << 12) | (patch << 0);
	}
} // namespace HyperEngine::Utils
