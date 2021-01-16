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
		HP_ASSERT(false, "The DirectX11 Context is not implemented!");
	}

	void DirectX11Context::PreInit()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void DirectX11Context::Init()
	{
	}

	void DirectX11Context::Shutdown()
	{
	}

	void DirectX11Context::OnResize(size_t width, size_t height)
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
