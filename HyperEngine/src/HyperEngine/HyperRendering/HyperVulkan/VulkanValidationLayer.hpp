#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "HyperRendering/HyperVulkan/Vulkan.hpp"

namespace HyperRendering
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
