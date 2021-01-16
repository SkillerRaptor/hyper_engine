#include "RenderContext.hpp"

#include <regex>

#if defined(HP_PLATFORM_WINDOWS)
	#include "Platform/Rendering/DirectX11/DirectX11Context.hpp"
	#include "Platform/Rendering/DirectX12/DirectX12Context.hpp"
#endif
#include "Platform/Rendering/OpenGL33/OpenGL33Context.hpp"
#include "Platform/Rendering/OpenGL46/OpenGL46Context.hpp"
#include "Platform/Rendering/Vulkan/VulkanContext.hpp"

namespace Hyperion
{
	Ref<RenderContext> RenderContext::Construct(GraphicsAPI graphicsAPI, std::string& title)
	{
		switch (graphicsAPI)
		{
		#if defined(HP_PLATFORM_WINDOWS)
		case GraphicsAPI::DIRECTX_11:
			return CreateRef<DirectX11Context>(graphicsAPI);
		case GraphicsAPI::DIRECTX_12:
			return CreateRef<DirectX12Context>(graphicsAPI);
		#endif
		case GraphicsAPI::OPENGL_33:
			return CreateRef<OpenGL33Context>(graphicsAPI);
		case GraphicsAPI::OPENGL_46:
			return CreateRef<OpenGL46Context>(graphicsAPI);
		case GraphicsAPI::VULKAN_12:
			return CreateRef<VulkanContext>(graphicsAPI);
		}

		HP_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
