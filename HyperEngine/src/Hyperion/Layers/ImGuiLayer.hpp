#pragma once

#include "Layer.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class ImGuiLayer : public Layer
	{
	private:
		uint32_t m_FrameTextureId;

	public:
		ImGuiLayer();
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		void SetFrameTextureId(uint32_t frameTextureId);
		uint32_t GetFrameTextureId() const;

	private:
		void SetupStyle();
		void ShowDockingMenu();
		void ShowMenuFile();
		void ShowMenuEdit();
	};
}
