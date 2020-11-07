#pragma once

#include "Renderer2D.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "HyperCore/Core.hpp"
#include "HyperUtilities/Timestep.hpp"

struct GLFWwindow;

namespace Hyperion
{
	struct FrameSize
	{
		uint32_t XPos;
		uint32_t YPos;

		uint32_t Width;
		uint32_t Height;
	};

	enum class GraphicsAPI
	{
		DIRECTX_11,
		DIRECTX_12,
		OPENGL_33,
		OPENGL_46,
		VULKAN_12
	};

	class RenderContext
	{
	protected:
		GLFWwindow* m_Window;

		FrameSize m_FrameSize;
		Ref<Renderer2D> m_Renderer2D;
		Ref<ShaderManager> m_ShaderManager;
		Ref<TextureManager> m_TextureManager;

	public:
		RenderContext()
			: m_Window(nullptr), m_FrameSize({ 0, 0, 0, 0 }) {}
		virtual ~RenderContext() = default;

		virtual void PreInit() = 0;
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void OnUpdate(Timestep timeStep) = 0;
		virtual void OnRender() = 0;

		void SetWindow(GLFWwindow* window) { m_Window = window; };

		FrameSize& GetFrameSize() { return m_FrameSize; }

		Ref<Renderer2D> GetRenderer2D() { return m_Renderer2D; };
		Ref<ShaderManager> GetShaderManager() { return m_ShaderManager; };
		Ref<TextureManager> GetTextureManager() { return m_TextureManager; };

		static Ref<RenderContext> Construct(GraphicsAPI graphicsAPI);
	};
}
