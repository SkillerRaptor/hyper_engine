#include "HyperRendering/RenderContext.hpp"

#if defined(HP_PLATFORM_WINDOWS)
	#include "HyperRendering/HyperDirectX11/DirectX11Context.hpp"
	#include "HyperRendering/HyperDirectX12/DirectX12Context.hpp"
#endif

#include "HyperRendering/HyperOpenGL33/OpenGL33Context.hpp"
#include "HyperRendering/HyperOpenGL46/OpenGL46Context.hpp"
#include "HyperRendering/HyperVulkan/VulkanContext.hpp"

namespace HyperRendering
{
	HyperCore::Ref<RenderContext> RenderContext::Construct(GraphicsAPI graphicsAPI, const std::string& title)
	{
		switch (graphicsAPI)
		{
		#if defined(HP_PLATFORM_WINDOWS)
		case GraphicsAPI::DIRECTX_11:
			return HyperCore::CreateRef<DirectX11Context>(graphicsAPI);
		case GraphicsAPI::DIRECTX_12:
			return HyperCore::CreateRef<DirectX12Context>(graphicsAPI);
		#endif
		case GraphicsAPI::OPENGL_33:
			return HyperCore::CreateRef<OpenGL33Context>(graphicsAPI);
		case GraphicsAPI::OPENGL_46:
			return HyperCore::CreateRef<OpenGL46Context>(graphicsAPI);
		case GraphicsAPI::VULKAN_12:
			return HyperCore::CreateRef<VulkanContext>(graphicsAPI);
		}

		HP_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
