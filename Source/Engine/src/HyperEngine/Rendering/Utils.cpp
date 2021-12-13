/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Utils.hpp"

#include "HyperEngine/Logger.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

namespace HyperEngine::Utils
{
	VkBool32 debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
		void *user_data)
	{
		HYPERENGINE_UNUSED_VARIABLE(type);
		HYPERENGINE_UNUSED_VARIABLE(user_data);

		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			Logger::warning("{}\n", callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			Logger::error("{}\n", callback_data->pMessage);
			break;
		default:
			break;
		}

		return VK_FALSE;
	}
} // namespace HyperEngine::Utils
