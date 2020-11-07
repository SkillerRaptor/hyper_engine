#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Vulkan.hpp"

namespace Hyperion
{
	class VulkanValidationLayer
	{
	private:
		VkInstance& m_Instance;

		VulkanValidationLayerInfo& m_ValidationLayerInfo;

	public:
		VulkanValidationLayer(VkInstance& instance, VulkanValidationLayerInfo& validationLayerInfo);

		void SetupInstanceDebugger(VkInstanceCreateInfo& instanceCreateInfo, VkDebugUtilsMessengerCreateInfoEXT& debugInstanceMessengerCreateInfo);
		void SetupValidationDebugger();
		void Shutdown();

	private:
		bool CheckValidationLayerSupport();
	};
}
