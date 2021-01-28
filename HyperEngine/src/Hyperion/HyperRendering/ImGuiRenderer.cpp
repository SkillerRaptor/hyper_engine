#include "ImGuiRenderer.hpp"

#if defined(HP_PLATFORM_WINDOWS)
//#include "Platform/Rendering/DirectX11/DirectX11ImGuiRenderer.hpp"
//#include "Platform/Rendering/DirectX12/DirectX12ImGuiRenderer.hpp"
#endif

#include "Platform/Rendering/OpenGL33/OpenGL33ImGuiRenderer.hpp"
#include "Platform/Rendering/OpenGL46/OpenGL46ImGuiRenderer.hpp"
//#include "Platform/Rendering/Vulkan/VulkanImGuiRenderer.hpp"

namespace Hyperion
{
	Ref<ImGuiRenderer> ImGuiRenderer::Construct(const Ref<RenderContext> renderContext)
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
			return CreateRef<OpenGL33ImGuiRenderer>();
		case GraphicsAPI::OPENGL_46:
			return CreateRef<OpenGL46ImGuiRenderer>();
			break;
		case GraphicsAPI::VULKAN_12:
			//return CreateRef<VulkanImGuiRenderer>();
			break;
		}

		HP_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
