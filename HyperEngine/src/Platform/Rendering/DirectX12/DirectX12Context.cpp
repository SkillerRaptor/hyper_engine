#include "DirectX12Context.hpp"

#include <GLFW/glfw3.h>

#include "DirectX12Renderer2D.hpp"
#include "DirectX12ShaderManager.hpp"
#include "DirectX12TextureManager.hpp"

namespace Hyperion
{
	DirectX12Context::DirectX12Context(GraphicsAPI graphicsAPI)
		: RenderContext(graphicsAPI)
	{
		HP_ASSERT(false, "The DirectX12 Context is not implemented!");
	}

	void DirectX12Context::PreInit()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void DirectX12Context::Init()
	{
	}

	void DirectX12Context::Shutdown()
	{
	}

	void DirectX12Context::OnResize(size_t width, size_t height)
	{
	}

	void DirectX12Context::OnPreUpdate()
	{
	}

	void DirectX12Context::OnUpdate(Timestep timeStep)
	{
	}

	void DirectX12Context::OnRender()
	{
	}
}
