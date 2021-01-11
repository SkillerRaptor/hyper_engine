#pragma once

#include "HyperCore/Core.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperRendering/ImGuiRenderer.hpp"

#include "Platform/Rendering/ImGui/EditorCamera.hpp"

namespace Hyperion
{
	class ImGuiLayer : public OverlayLayer
	{
	private:
		Ref<ImGuiRenderer> m_ImGuiRenderer;
		Ref<EditorCamera> m_EditorCamera;
			 
	public:
		ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& event) override;

		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		void Start();
		void End();

	private:
		void SetupStyle();
	};
}
