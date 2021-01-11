#include "SceneRecorder.hpp"

#if defined(HP_PLATFORM_WINDOWS)
	//#include "Platform/Rendering/DirectX11/DirectX11SceneRecorder.hpp"
	//#include "Platform/Rendering/DirectX12/DirectX12SceneRecorder.hpp"
#endif

#include "Platform/Rendering/OpenGL33/OpenGL33SceneRecorder.hpp"
//#include "Platform/Rendering/OpenGL46/OpenGL46SceneRecorder.hpp"
//#include "Platform/Rendering/Vulkan/VulkanSceneRecorder.hpp"

namespace Hyperion
{
	Ref<SceneRecorder> SceneRecorder::Construct(Ref<RenderContext> renderContext)
	{
		switch (renderContext->GetGraphicsAPI())
		{
		#ifdef HP_PLATFORM_WINDOWS
		case GraphicsAPI::DIRECTX_11:
			//return CreateRef<DirectX11SceneRecorder>(renderContext);
			break;
		case GraphicsAPI::DIRECTX_12:
			//return CreateRef<DirectX12SceneRecorder>(renderContext);
			break;
		#endif
		case GraphicsAPI::OPENGL_33:
			return CreateRef<OpenGL33SceneRecorder>(renderContext);
		case GraphicsAPI::OPENGL_46:
			//return CreateRef<OpenGL46SceneRecorder>(renderContext);
			break;
		case GraphicsAPI::VULKAN_12:
			//return CreateRef<VulkanSceneRecorder>(renderContext);
			break;
		}

		HP_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
