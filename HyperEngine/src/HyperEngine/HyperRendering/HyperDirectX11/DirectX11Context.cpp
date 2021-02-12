#include "HyperRendering/HyperDirectX11/DirectX11Context.hpp"

#include <GLFW/glfw3.h>

#include "HyperRendering/HyperDirectX11/DirectX11Renderer2D.hpp"
#include "HyperRendering/HyperDirectX11/DirectX11ShaderManager.hpp"
#include "HyperRendering/HyperDirectX11/DirectX11TextureManager.hpp"

namespace HyperRendering
{
	DirectX11Context::DirectX11Context(GraphicsAPI graphicsAPI)
		: RenderContext{ graphicsAPI }
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

	void DirectX11Context::OnUpdate(HyperUtilities::Timestep timeStep)
	{
	}

	void DirectX11Context::OnRender()
	{
	}
}
