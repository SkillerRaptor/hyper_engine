#pragma once

#include <imgui.h>

#include "ImGuiRenderer.hpp"
#include "HyperCore/Core.hpp"
#include "HyperECS/Entity.hpp"
#include "HyperECS/Scene/Scene.hpp"
#include "HyperECS/Scene/SceneHierarchyPanel.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class ImGuiLayer : public OverlayLayer
	{
	private:
		Ref<ImGuiRenderer> m_ImGuiRenderer;

		Ref<Scene> m_Scene;
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
		ImFont* m_Font;
		Entity m_CameraEntity = { 0, nullptr };

	public:
		ImGuiLayer(Ref<Scene> scene);

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& event) override;
		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		void InitCapture();
		void StartCapture();
		void EndCapture();

	private:
		void SetupStyle();

		void DrawSelection();

		void ShowDockingMenu();
		void ShowMenuFile();
		void ShowMenuEdit();

		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveAsScene();
	};
}
