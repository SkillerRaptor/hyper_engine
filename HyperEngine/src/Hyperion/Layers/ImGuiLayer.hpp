#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

#include "Layer.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timeStep) override;
		void OnRender(unsigned int frameTexture);

	private:
		void SetupStyle();
		void ShowDockingMenu();
		void ShowMenuFile();
		void ShowMenuEdit();
	};
}