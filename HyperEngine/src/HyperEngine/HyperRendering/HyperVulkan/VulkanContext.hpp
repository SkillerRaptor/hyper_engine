#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "HyperRendering/RenderContext.hpp"
#include "HyperRendering/HyperVulkan/Vulkan.hpp"
#include "HyperRendering/HyperVulkan/VulkanValidationLayer.hpp"

namespace HyperRendering
{
	class VulkanContext : public RenderContext
	{
	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;

		VulkanValidationLayer m_ValidationLayer;
		VulkanValidationLayerInfo m_ValidationLayerInfo;

	public:
		VulkanContext(GraphicsAPI graphicsAPI);

		virtual void PreInit() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnResize(size_t width, size_t height) override;

		virtual void OnPreUpdate() override;
		virtual void OnUpdate(HyperUtilities::Timestep timeStep) override;
		virtual void OnRender() override;
	};
}
