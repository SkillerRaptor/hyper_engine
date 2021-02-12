#pragma once

#include "HyperCore/Core.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperRendering/EditorCamera.hpp"
#include "HyperRendering/ImGuiRenderer.hpp"

namespace HyperRendering
{
	class ImGuiLayer : public HyperLayer::OverlayLayer
	{
	private:
		HyperCore::Ref<ImGuiRenderer> m_ImGuiRenderer;
		EditorCamera m_EditorCamera;

	public:
		ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(HyperUtilities::Timestep timeStep) override;

		virtual void OnEvent(HyperEvent::Event& event) override;

		void Start();
		void End();

		inline EditorCamera& GetEditorCamera()
		{
			return m_EditorCamera;
		}

	private:
		void SetupStyle();
	};
}
