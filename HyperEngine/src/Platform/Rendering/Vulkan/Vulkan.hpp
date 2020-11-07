#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace Hyperion
{
	struct VulkanValidationLayerInfo
	{
		VkDebugUtilsMessengerEXT DebugMessenger;
		bool ValidationLayersEnabled;
		const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
	};
}
