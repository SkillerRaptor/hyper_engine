#include "HyperRendering/HyperVulkan/VulkanContext.hpp"

#ifdef HP_SUPPORT_VULKAN

#include <vector>

#include "HyperRendering/HyperVulkan/VulkanRenderer2D.hpp"
#include "HyperRendering/HyperVulkan/VulkanShaderManager.hpp"
#include "HyperRendering/HyperVulkan/VulkanTextureManager.hpp"
#include "HyperRendering/HyperVulkan/VulkanValidationLayer.hpp"

namespace HyperRendering
{
	VulkanContext::VulkanContext(GraphicsAPI graphicsAPI)
		: RenderContext{ graphicsAPI }, m_ValidationLayerInfo{}, m_ValidationLayer{ m_Instance, m_ValidationLayerInfo }
	{
		HP_ASSERT(false, "The Vulkan Context implementation is not finished!");
	}

	void VulkanContext::PreInit()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void VulkanContext::Init()
	{
	#if defined(HP_SUPPORT_VULKAN)
		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "HyperEngine";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "HyperEngine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_0;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_ValidationLayerInfo.ValidationLayersEnabled)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;

		VkDebugUtilsMessengerCreateInfoEXT debugInstanceMessengerCreateInfo{};
		m_ValidationLayer.SetupInstanceDebugger(instanceCreateInfo, debugInstanceMessengerCreateInfo);

		HP_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to create Vulkan �nstance!");

		m_ValidationLayer.SetupValidationDebugger();

		HP_ASSERT(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) == VK_SUCCESS, "Failed to create Vulkan window surface!");

	#endif
	}

	void VulkanContext::Shutdown()
	{
	#if defined(HP_SUPPORT_VULKAN)
		m_ValidationLayer.Shutdown();

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	#endif
	}

	void VulkanContext::OnResize(size_t width, size_t height)
	{
	}

	void VulkanContext::OnPreUpdate()
	{
	}

	void VulkanContext::OnUpdate(HyperUtilities::Timestep timeStep)
	{
	}

	void VulkanContext::OnRender()
	{
	}
}

#endif
