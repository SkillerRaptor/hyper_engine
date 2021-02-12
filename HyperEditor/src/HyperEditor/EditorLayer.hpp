#pragma once

#include <HyperEngine.hpp>

#include "HyperPanels/AssetsPanel.hpp"
#include "HyperPanels/SceneHierarchyPanel.hpp"

namespace HyperEditor
{
	class EditorLayer : public HyperLayer::OverlayLayer
	{
	private:
		HyperRendering::ShaderHandle m_SpriteShader{ 0 }; // FIXME: TEMP

		HyperCore::Scope<AssetsPanel> m_AssetsPanel;
		HyperCore::Scope<SceneHierarchyPanel> m_SceneHierarchyPanel;

	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;

		virtual void OnRender() override;

	private:
		void CreateDockingMenu();

		void CreateMenuBar();
		void CreateMenuFile();
		void CreateMenuEdit();

		void CreateNewScene();
		void OpenScene();
		void SaveScene();
		void SaveAsScene();
	};
}
