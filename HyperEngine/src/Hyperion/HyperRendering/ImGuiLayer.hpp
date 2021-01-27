#pragma once

#include "HyperCore/Core.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperRendering/EditorCamera.hpp"
#include "HyperRendering/ImGuiRenderer.hpp"

namespace Hyperion
{
	class ImGuiLayer : public OverlayLayer
	{
	private:
		Ref<ImGuiRenderer> m_ImGuiRenderer;
		EditorCamera m_EditorCamera;

	public:
		ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& event) override;

		virtual void OnUpdate(Timestep timeStep) override;

		void Start();
		void End();

		EditorCamera& GetEditorCamera();

	private:
		void SetupStyle();
	};
}
