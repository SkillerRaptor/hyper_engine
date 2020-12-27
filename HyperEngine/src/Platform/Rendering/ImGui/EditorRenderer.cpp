#include "EditorRenderer.hpp"

#ifdef HP_PLATFORM_WINDOWS
//#include "Platform/Rendering/DirectX11/DirectX11EditorRenderer.hpp"
//#include "Platform/Rendering/DirectX12/DirectX12EditorRenderer.hpp"
#endif
#include "Platform/Rendering/OpenGL33/OpenGL33EditorRenderer.hpp"
#include "Platform/Rendering/OpenGL46/OpenGL46EditorRenderer.hpp"
//#include "Platform/Rendering/Vulkan/VulkanEditorRenderer.hpp"

namespace Hyperion
{
	Ref<EditorRenderer> EditorRenderer::Construct(Ref<RenderContext> renderContext)
	{
		// TODO: Adding ImGui Render for DirectX11, DirectX12, OpenGL 4.6, Vulkan
		switch (renderContext->GetGraphicsAPI())
		{
		case GraphicsAPI::DIRECTX_11:
			//return CreateRef<DirectX11EditorRenderer>(renderContext);
			break;
		case GraphicsAPI::DIRECTX_12:
			//return CreateRef<DirectX12EditorRenderer>(renderContext);
			break;
		case GraphicsAPI::OPENGL_33:
			return CreateRef<OpenGL33EditorRenderer>(renderContext);
		case GraphicsAPI::OPENGL_46:
			return CreateRef<OpenGL46EditorRenderer>(renderContext);
		case GraphicsAPI::VULKAN_12:
			//return CreateRef<VulkanEditorRenderer>(renderContext);
			break;
		}

		HP_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
