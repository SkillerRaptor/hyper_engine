#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <queue>

#include "RenderContext.hpp"
#include "Core/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	struct WindowData
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		uint32_t XPos;
		uint32_t YPos;

		bool VSync;
		std::queue<Ref<Event>>* EventBus;

		WindowData() = default;
	};

	class Window
	{
	protected:
		RenderContext* m_Context;
		WindowData m_Data;

	public:
		Window(const WindowData& windowData)
			: m_Context(nullptr), m_Data(windowData) {}
		virtual ~Window() = default;

		virtual void InitWindow() = 0;
		virtual void ShutdownWindow() = 0;

		virtual void OnTick(int currentTick) = 0;
		virtual void OnUpdate(Timestep timeStep) = 0;
		virtual void OnRender() = 0;

		virtual void SetTitle(std::string title) = 0;
		virtual std::string GetTitle() const = 0;

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

		virtual void* GetWindow() const = 0;
		virtual RenderContext* GetContext() const = 0;
		virtual const WindowData& GetWindowData() const = 0;

		static Ref<Window> Construct(const WindowData& windowData);
	};
}
