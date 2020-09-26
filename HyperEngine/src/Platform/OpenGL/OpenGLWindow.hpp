#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "OpenGLContext.hpp"
#include "Core/Core.hpp"
#include "Rendering/Window.hpp"

namespace Hyperion
{
	class OpenGLWindow : public Window
	{
	private:
		GLFWwindow* m_Window;

	public:
		OpenGLWindow(std::string title, uint32_t width, uint32_t height, bool vSync, std::queue<Ref<Event>>* eventBus);
		~OpenGLWindow();

		virtual void InitWindow() override;
		virtual void ShutdownWindow() override;

		virtual void OnTick(int currentTick) override;
		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		virtual void SetTitle(std::string title) override;
		virtual std::string GetTitle() const override;

		virtual void SetWidth(uint32_t width) override;
		virtual uint32_t GetWidth() const override;

		virtual void SetHeight(uint32_t height) override;
		virtual uint32_t GetHeight() const override;

		virtual void SetXPos(uint32_t xPos) override;
		virtual uint32_t GetXPos() const override;

		virtual void SetYPos(uint32_t yPos) override;
		virtual uint32_t GetYPos() const override;

		virtual void SetVSync(bool vSync) override;
		virtual bool IsVSync() const override;

		virtual void* GetWindow() const override;
		virtual RenderContext* GetContext() const override;
		const virtual WindowData& GetWindowData() const override;
	};
}