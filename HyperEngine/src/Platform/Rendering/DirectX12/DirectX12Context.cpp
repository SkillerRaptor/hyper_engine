#include "DirectX12Context.hpp"

#include <GLFW/glfw3.h>

#include "DirectX12Renderer2D.hpp"
#include "DirectX12ShaderManager.hpp"
#include "DirectX12TextureManager.hpp"

namespace Hyperion
{
	DirectX12Context::DirectX12Context()
	{
	}

	void DirectX12Context::PreInit()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void DirectX12Context::Init()
	{
		// TODO: DirectX12 Stuff
		//m_Renderer2D = CreateRef<DirectX12Renderer2D>();
		//m_ShaderManager = CreateRef<DirectX12ShaderManager>();
		//m_TextureManager = CreateRef<DirectX12TextureManager>();
	}

	void DirectX12Context::Shutdown()
	{
	}

	void DirectX12Context::OnTick(int currentTick)
	{
	}

	void DirectX12Context::OnUpdate(Timestep timeStep)
	{
	}

	void DirectX12Context::OnRender()
	{
	}
}
