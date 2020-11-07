#include "DirectX11Context.hpp"

#include <GLFW/glfw3.h>

#include "DirectX11Renderer2D.hpp"
#include "DirectX11ShaderManager.hpp"
#include "DirectX11TextureManager.hpp"

namespace Hyperion
{
	DirectX11Context::DirectX11Context(GraphicsAPI graphicsAPI)
		: RenderContext(graphicsAPI)
	{
	}

	void DirectX11Context::PreInit()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void DirectX11Context::Init()
	{
		// TODO: DirectX11 Stuff
		//m_Renderer2D = CreateRef<DirectX11Renderer2D>();
		//m_ShaderManager = CreateRef<DirectX11ShaderManager>();
		//m_TextureManager = CreateRef<DirectX11TextureManager>();
	}

	void DirectX11Context::Shutdown()
	{
	}

	void DirectX11Context::OnPreUpdate()
	{
	}

	void DirectX11Context::OnUpdate(Timestep timeStep)
	{
	}

	void DirectX11Context::OnRender()
	{
	}
}
