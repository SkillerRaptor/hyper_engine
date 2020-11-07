#pragma once

#include <iostream>
#include <queue>

#include "RenderContext.hpp"
#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	struct WindowPropsInfo
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		std::queue<Ref<Event>>* EventBus = nullptr;
	};

	class Window
	{
	protected:
		struct WindowDataInfo
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;

			uint32_t XPos;
			uint32_t YPos;

			bool VSync;
			std::queue<Ref<Event>>* EventBus;

			WindowDataInfo(const WindowPropsInfo& windowPropsInfo)
				: Title(windowPropsInfo.Title), Width(windowPropsInfo.Width), Height(windowPropsInfo.Height), XPos(-1), YPos(-1), VSync(false), EventBus(windowPropsInfo.EventBus) {}
		};

		Ref<RenderContext> m_RenderContext;
		GraphicsAPI m_GraphicsAPI;
		WindowDataInfo m_WindowDataInfo;

	public:
		Window(const WindowPropsInfo& windowProps)
			: m_RenderContext(nullptr), m_GraphicsAPI(GraphicsAPI::OPENGL_33), m_WindowDataInfo(windowProps) {}
		virtual ~Window() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void OnTick(int currentTick) = 0;
		virtual void OnUpdate(Timestep timeStep) = 0;
		virtual void OnRender() = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual const std::string& GetTitle() const = 0;

		virtual void SetWidth(uint32_t width) = 0;
		virtual uint32_t GetWidth() const = 0;

		virtual void SetHeight(uint32_t height) = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetXPos(uint32_t xPos) = 0;
		virtual uint32_t GetXPos() const = 0;

		virtual void SetYPos(uint32_t yPos) = 0;
		virtual uint32_t GetYPos() const = 0;

		virtual void SetVSync(bool vSync) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetWindow() = 0;
		Ref<RenderContext> GetContext() const { return m_RenderContext; };
		const WindowDataInfo& GetWindowDataInfo() const { return m_WindowDataInfo; };

		static Ref<Window> Construct(const WindowPropsInfo& windowPropsInfo);
	};
}
