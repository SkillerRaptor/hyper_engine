#pragma once

#include "HyperCore/Core.hpp"

#ifdef HP_PLATFORM_WINDOWS

#include "HyperRendering/Window.hpp"

struct GLFWwindow;

namespace HyperSystem
{
	class WindowsWindow : public HyperRendering::Window
	{
	private:
		GLFWwindow* m_Window;

	public:
		WindowsWindow(const HyperRendering::WindowPropsInfo& windowPropsInfo);
		~WindowsWindow();

		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnPreUpdate() override;
		virtual void OnUpdate(HyperUtilities::Timestep timeStep) override;
		virtual void OnRender() override;

		virtual void SetTitle(const std::string& title) override;
		virtual const std::string& GetTitle() const override;

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

		virtual void SetAppIcon(const std::string& imagePath) override;

		virtual double GetTime() override;
	};
}

#endif
