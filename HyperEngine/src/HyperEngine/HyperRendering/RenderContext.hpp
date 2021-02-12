#pragma once

#include <queue>

#include "HyperCore/Core.hpp"
#include "HyperEvent/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperRendering/SceneRecorder.hpp"
#include "HyperRendering/ShaderManager.hpp"
#include "HyperRendering/TextureManager.hpp"
#include "HyperUtilities/Timestep.hpp"

struct GLFWwindow;

namespace HyperRendering
{
	enum class GraphicsAPI
	{
		DIRECTX_11,
		DIRECTX_12,
		OPENGL_33,
		OPENGL_46,
		VULKAN_12
	};

	class RenderContext;

	struct WindowDataInfo
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		uint32_t XPos;
		uint32_t YPos;

		HyperCore::Ref<RenderContext> Context;

		bool VSync;
		std::queue<HyperCore::Ref<HyperEvent::Event>>* EventBus;
	};

	class RenderContext
	{
	protected:
		GLFWwindow* m_Window;
		GraphicsAPI m_GraphicsAPI;

		HyperCore::Ref<SceneRecorder> m_SceneRecorder;
		HyperCore::Ref<Renderer2D> m_Renderer2D;
		HyperCore::Ref<ShaderManager> m_ShaderManager;
		HyperCore::Ref<TextureManager> m_TextureManager;

		friend class WindowsWindow;

	public:
		virtual ~RenderContext() = default;

		virtual void PreInit() = 0;
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void OnResize(size_t width, size_t height) = 0;

		virtual void OnPreUpdate() = 0;
		virtual void OnUpdate(HyperUtilities::Timestep timeStep) = 0;
		virtual void OnRender() = 0;

		inline void SetWindow(GLFWwindow* window)
		{
			m_Window = window;
		}

		inline GLFWwindow* GetWindow() const
		{
			return m_Window;
		}

		inline GraphicsAPI GetGraphicsAPI() const
		{
			return m_GraphicsAPI;
		}

		inline HyperCore::Ref<SceneRecorder> GetSceneRecorder()
		{
			return m_SceneRecorder;
		}

		inline HyperCore::Ref<Renderer2D> GetRenderer2D()
		{
			return m_Renderer2D;
		}

		inline HyperCore::Ref<ShaderManager> GetShaderManager()
		{
			return m_ShaderManager;
		}

		inline HyperCore::Ref<TextureManager> GetTextureManager()
		{
			return m_TextureManager;
		}

		static HyperCore::Ref<RenderContext> Construct(GraphicsAPI graphicsAPI, const std::string& title);

	protected:
		RenderContext(GraphicsAPI graphicsAPI)
			: m_Window{ nullptr }, m_GraphicsAPI{ graphicsAPI }
		{
		}
	};
}
