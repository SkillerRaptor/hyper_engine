#pragma once

#include <queue>

#include "Renderer2D.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperUtilities/Timestep.hpp"

struct GLFWwindow;

namespace Hyperion
{
	struct ImGuiFrameSizeInfo
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

	struct WindowDataInfo
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		uint32_t XPos;
		uint32_t YPos;

		bool VSync;
		std::queue<Ref<Event>>* EventBus;
	};

	class RenderContext
	{
	protected:
		GLFWwindow* m_Window;
		GraphicsAPI m_GraphicsAPI;

		ImGuiFrameSizeInfo m_ImGuiEditorSizeInfo;
		ImGuiFrameSizeInfo m_ImGuiGameSizeInfo;

		Ref<Renderer2D> m_Renderer2D;
		Ref<ShaderManager> m_ShaderManager;
		Ref<TextureManager> m_TextureManager;

		friend class WindowsWindow;

	public:
		virtual ~RenderContext() = default;

		virtual void PreInit() = 0;
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void OnPreUpdate() = 0;
		virtual void OnUpdate(Timestep timeStep) = 0;
		virtual void OnRender() = 0;

		GLFWwindow* GetWindow() const { return m_Window; };
		GraphicsAPI GetGraphicsAPI() const { return m_GraphicsAPI; };

		ImGuiFrameSizeInfo& GetImGuiEditorSizeInfo() { return m_ImGuiEditorSizeInfo; }
		ImGuiFrameSizeInfo& GetImGuiGameSizeInfo() { return m_ImGuiGameSizeInfo; }

		Ref<Renderer2D> GetRenderer2D() { return m_Renderer2D; };
		Ref<ShaderManager> GetShaderManager() { return m_ShaderManager; };
		Ref<TextureManager> GetTextureManager() { return m_TextureManager; };

		static Ref<RenderContext> Construct(GraphicsAPI graphicsAPI, std::string& title);

	protected:
		RenderContext(GraphicsAPI graphicsAPI)
			: m_Window(nullptr), m_GraphicsAPI(graphicsAPI), m_ImGuiEditorSizeInfo({}), m_ImGuiGameSizeInfo({}) {}
	};
}
