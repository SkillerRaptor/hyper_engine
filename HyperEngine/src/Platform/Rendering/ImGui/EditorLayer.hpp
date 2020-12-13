#pragma once

#include <imgui.h>

#include "EditorCamera.hpp"
#include "EditorRenderer.hpp"
#include "SceneHierarchyPanel.hpp"
#include "HyperCore/Core.hpp"
#include "HyperECS/HyperEntity.hpp"
#include "HyperECS/Scene/Scene.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class EditorLayer : public OverlayLayer
	{
	private:
		Ref<EditorRenderer> m_EditorRenderer;

		Ref<Scene> m_Scene;
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
		ImFont* m_Font;

		Ref<EditorCamera> m_EditorCamera;

	public:
		EditorLayer(Ref<Scene> scene);

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

		void ShowAssetsMenu();

		void ShowDockingMenu();
		void ShowMenuFile();
		void ShowMenuEdit();

		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveAsScene();
	};
}
