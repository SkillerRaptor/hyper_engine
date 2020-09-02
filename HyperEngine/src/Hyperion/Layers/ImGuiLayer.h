#pragma once

#include "Layer.h"
#include "Utilities/Timestep.h"

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