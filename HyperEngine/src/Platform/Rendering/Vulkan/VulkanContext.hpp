#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Vulkan.hpp"
#include "VulkanValidationLayer.hpp"
#include "HyperRendering/RenderContext.hpp"

namespace Hyperion
{
	class VulkanContext : public RenderContext
	{
	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;

		VulkanValidationLayer m_ValidationLayer;
		VulkanValidationLayerInfo m_ValidationLayerInfo;

	public:
		VulkanContext();

		virtual void PreInit() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;
	};
}
