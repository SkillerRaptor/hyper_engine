#include "HyperRendering/RenderContext.hpp"

#ifdef HP_PLATFORM_WINDOWS
	#include "HyperRendering/HyperDirectX11/DirectX11Context.hpp"
	#include "HyperRendering/HyperDirectX12/DirectX12Context.hpp"
#endif

#include "HyperRendering/HyperOpenGL33/OpenGL33Context.hpp"
#include "HyperRendering/HyperOpenGL46/OpenGL46Context.hpp"

#ifdef HP_SUPPORT_VULKAN
	#include "HyperRendering/HyperVulkan/VulkanContext.hpp"
#endif

namespace HyperRendering
{
	HyperCore::Ref<RenderContext> RenderContext::Construct(GraphicsAPI graphicsAPI, const std::string& title)
	{
		switch (graphicsAPI)
		{
		#ifdef HP_PLATFORM_WINDOWS
		case GraphicsAPI::DIRECTX_11:
			return HyperCore::CreateRef<DirectX11Context>(graphicsAPI);
		case GraphicsAPI::DIRECTX_12:
			return HyperCore::CreateRef<DirectX12Context>(graphicsAPI);
		#endif
		case GraphicsAPI::OPENGL_33:
			return HyperCore::CreateRef<OpenGL33Context>(graphicsAPI);
		case GraphicsAPI::OPENGL_46:
			return HyperCore::CreateRef<OpenGL46Context>(graphicsAPI);
		#ifdef HP_SUPPORT_VULKAN
		case GraphicsAPI::VULKAN_12:
			return HyperCore::CreateRef<VulkanContext>(graphicsAPI);
		#endif
		}

		HP_ASSERT(false, "Couldn't create the render context - Unknown graphics api!");
		return nullptr;
	}
}
