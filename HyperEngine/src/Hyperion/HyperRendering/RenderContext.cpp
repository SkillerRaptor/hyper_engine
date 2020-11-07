#include "RenderContext.hpp"

#include "HyperCore/Core.hpp"

#include "Platform/Rendering/DirectX11/DirectX11Context.hpp"
#include "Platform/Rendering/DirectX12/DirectX12Context.hpp"
#include "Platform/Rendering/OpenGL33/OpenGL33Context.hpp"
#include "Platform/Rendering/OpenGL46/OpenGL46Context.hpp"
#include "Platform/Rendering/Vulkan/VulkanContext.hpp"

namespace Hyperion
{
	Ref<RenderContext> RenderContext::Construct(GraphicsAPI graphicsAPI)
	{
		switch (graphicsAPI)
		{
		case GraphicsAPI::DIRECTX_11:
			return CreateRef<DirectX11Context>();
		case GraphicsAPI::DIRECTX_12:
			return CreateRef<DirectX12Context>();
		case GraphicsAPI::OPENGL_33:
			return CreateRef<OpenGL33Context>();
		case GraphicsAPI::OPENGL_46:
			return CreateRef<OpenGL46Context>();
		case GraphicsAPI::VULKAN_12:
			return CreateRef<VulkanContext>();
		}

		HP_CORE_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
