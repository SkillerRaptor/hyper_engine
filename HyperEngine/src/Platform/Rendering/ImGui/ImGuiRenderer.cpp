#include "ImGuiRenderer.hpp"

//#include "Platform/Rendering/DirectX11/DirectX11ImGuiRenderer.hpp"
//#include "Platform/Rendering/DirectX12/DirectX12ImGuiRenderer.hpp"
#include "Platform/Rendering/OpenGL33/OpenGL33ImGuiRenderer.hpp"
#include "Platform/Rendering/OpenGL46/OpenGL46ImGuiRenderer.hpp"
//#include "Platform/Rendering/Vulkan/VulkanImGuiRenderer.hpp"

namespace Hyperion
{
	Ref<ImGuiRenderer> ImGuiRenderer::Construct(Ref<RenderContext> renderContext)
	{
		// TODO: Adding ImGui Render for DirectX11, DirectX12, OpenGL 4.6, Vulkan
		switch (renderContext->GetGraphicsAPI())
		{
		case GraphicsAPI::DIRECTX_11:
			//return CreateRef<DirectX11ImGuiRenderer>(renderContext);
			break;
		case GraphicsAPI::DIRECTX_12:
			//return CreateRef<DirectX12ImGuiRenderer>(renderContext);
			break;
		case GraphicsAPI::OPENGL_33:
			return CreateRef<OpenGL33ImGuiRenderer>(renderContext);
		case GraphicsAPI::OPENGL_46:
			return CreateRef<OpenGL46ImGuiRenderer>(renderContext);
		case GraphicsAPI::VULKAN_12:
			//return CreateRef<VulkanImGuiRenderer>(renderContext);
			break;
		}

		HP_CORE_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
