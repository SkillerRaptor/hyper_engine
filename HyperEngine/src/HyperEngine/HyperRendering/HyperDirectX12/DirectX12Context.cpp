#include "HyperRendering/HyperDirectX12/DirectX12Context.hpp"

#ifdef HP_PLATFORM_WINDOWS

#include <GLFW/glfw3.h>

#include "HyperRendering/HyperDirectX12/DirectX12Renderer2D.hpp"
#include "HyperRendering/HyperDirectX12/DirectX12ShaderManager.hpp"
#include "HyperRendering/HyperDirectX12/DirectX12TextureManager.hpp"

namespace HyperRendering
{
	DirectX12Context::DirectX12Context(GraphicsAPI graphicsAPI)
		: RenderContext{ graphicsAPI }
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

	void DirectX12Context::OnUpdate(HyperUtilities::Timestep timeStep)
	{
	}

	void DirectX12Context::OnRender()
	{
	}
}

#endif
