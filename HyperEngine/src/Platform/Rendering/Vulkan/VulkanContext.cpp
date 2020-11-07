#include "VulkanContext.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderer2D.hpp"
#include "VulkanShaderManager.hpp"
#include "VulkanTextureManager.hpp"

namespace Hyperion
{
	VulkanContext::VulkanContext()
	{
	}

	void VulkanContext::PreInit()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void VulkanContext::Init()
	{
		// TODO: Vulkan Stuff
		//m_Renderer2D = CreateRef<VulkanRenderer2D>();
		//m_ShaderManager = CreateRef<VulkanShaderManager>();
		//m_TextureManager = CreateRef<VulkanTextureManager>();
	}

	void VulkanContext::Shutdown()
	{
	}

	void VulkanContext::OnTick(int currentTick)
	{
	}

	void VulkanContext::OnUpdate(Timestep timeStep)
	{
	}

	void VulkanContext::OnRender()
	{
	}
}
