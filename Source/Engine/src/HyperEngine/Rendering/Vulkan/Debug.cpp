/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Debug.hpp"

#include "HyperEngine/Core/Logger.hpp"

namespace HyperEngine::Vulkan
{
	VKAPI_ATTR auto VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
		VkDebugUtilsMessageTypeFlagsEXT type_flags,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data) -> VkBool32
	{
		HYPERENGINE_VARIABLE_NOT_USED(type_flags);
		HYPERENGINE_VARIABLE_NOT_USED(user_data);

		switch (severity_flags)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			CLogger::warning("{}", callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			CLogger::error("{}", callback_data->pMessage);
			break;
		default:
			break;
		}

		return VK_FALSE;
	}
} // namespace HyperEngine::Vulkan
