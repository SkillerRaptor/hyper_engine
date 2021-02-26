#pragma once

#ifdef HP_SUPPORT_VULKAN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace HyperRendering
{
	struct VulkanValidationLayerInfo
	{
		VkDebugUtilsMessengerEXT DebugMessenger;
		bool ValidationLayersEnabled;
		const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
	};
}

#endif
