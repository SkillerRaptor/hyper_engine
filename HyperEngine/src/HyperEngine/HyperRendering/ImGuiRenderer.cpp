#include "HyperRendering/ImGuiRenderer.hpp"

#if defined(HP_PLATFORM_WINDOWS)
	//#include "HyperRendering/HyperDirectX11/DirectX11ImGuiRenderer.hpp"
	//#include "HyperRendering/HyperDirectX12/DirectX12ImGuiRenderer.hpp"
#endif

#include "HyperRendering/HyperOpenGL33/OpenGL33ImGuiRenderer.hpp"
#include "HyperRendering/HyperOpenGL46/OpenGL46ImGuiRenderer.hpp"
//#include "HyperRendering/HyperVulkan/VulkanImGuiRenderer.hpp"

namespace HyperRendering
{
	HyperCore::Ref<ImGuiRenderer> ImGuiRenderer::Construct(const HyperCore::Ref<RenderContext> renderContext)
	{
		switch (renderContext->GetGraphicsAPI())
		{
		#ifdef HP_PLATFORM_WINDOWS
		case GraphicsAPI::DIRECTX_11:
			//return CreateRef<DirectX11ImGuiRenderer>();
			break;
		case GraphicsAPI::DIRECTX_12:
			//return CreateRef<DirectX12ImGuiRenderer>();
			break;
		#endif
		case GraphicsAPI::OPENGL_33:
			return HyperCore::CreateRef<OpenGL33ImGuiRenderer>();
		case GraphicsAPI::OPENGL_46:
			return HyperCore::CreateRef<OpenGL46ImGuiRenderer>();
			break;
		case GraphicsAPI::VULKAN_12:
			//return CreateRef<VulkanImGuiRenderer>();
			break;
		}

		HP_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
