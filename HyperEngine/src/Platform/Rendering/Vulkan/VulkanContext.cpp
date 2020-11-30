#include "VulkanContext.hpp"

#include <vector>

#include "VulkanRenderer2D.hpp"
#include "VulkanShaderManager.hpp"
#include "VulkanTextureManager.hpp"
#include "VulkanValidationLayer.hpp"

namespace Hyperion
{
	VulkanContext::VulkanContext(GraphicsAPI graphicsAPI)
		: RenderContext(graphicsAPI), m_ValidationLayer(m_Instance, m_ValidationLayerInfo)
	{
	}

	void VulkanContext::PreInit()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void VulkanContext::Init()
	{
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

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			std::cerr << " Failed to create Vulkan Instance!" << std::endl;
			std::exit(-1);
		}

		m_ValidationLayer.SetupValidationDebugger();

		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
		{
			std::cerr << " Failed to create Window Surface!" << std::endl;
			std::exit(-1);
		}

		// TODO: Vulkan Stuff
		//m_Renderer2D = CreateRef<VulkanRenderer2D>();
		//m_ShaderManager = CreateRef<VulkanShaderManager>();
		//m_TextureManager = CreateRef<VulkanTextureManager>();
	}

	void VulkanContext::Shutdown()
	{
		m_ValidationLayer.Shutdown();

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::OnResize(size_t width, size_t height)
	{

	}

	void VulkanContext::OnPreUpdate()
	{
	}

	void VulkanContext::OnUpdate(Timestep timeStep)
	{
	}

	void VulkanContext::OnRender()
	{
	}
}
