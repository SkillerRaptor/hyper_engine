#pragma once

#include "Context.hpp"

#include <HyperEvent/EventManager.hpp>
#include <HyperMath/Vector2.hpp>

#include <string>

struct GLFWwindow;

namespace HyperEngine
{
	struct WindowInfo
	{
		std::string title;
		
		Vec2ui position;
		uint32_t width;
		uint32_t height;
		
		bool isDecorated;
		bool isResizable;
		bool isVsync;
		bool isShown;
		bool isFocused;
		
		EventManager* pEventManager;
	};
	
	class Window
	{
	public:
		explicit Window(WindowInfo windowInfo);
		~Window();
		
		void Present();
		
		void SetTitle(const std::string& title);
		const std::string& GetTitle() const;
		
		void SetPosition(Vec2ui position);
		Vec2ui GetPosition() const;
		
		void SetWidth(uint32_t width);
		uint32_t GetWidth() const;
		
		void SetHeight(uint32_t height);
		uint32_t GetHeight() const;
		
		void SetDecorated(bool isDecorated);
		bool IsDecorated() const;
		
		void SetResizable(bool isResizable);
		bool IsResizable() const;
		
		void SetVsync(bool isVsync);
		bool IsVsync() const;
		
		void SetShown(bool isShown);
		bool IsShown() const;
		
		void SetFocused(bool isFocused);
		bool IsFocused() const;
		
		Vec2ui GetFramebufferSize() const;
		float GetTime() const;
		
	private:
		GLFWwindow* m_pWindow;
		Context* m_pRenderContext;
		
		WindowInfo m_windowInfo;
	};
}
