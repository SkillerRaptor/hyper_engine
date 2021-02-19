#pragma once

#include <HyperEngine.hpp>

#include "HyperPanels/AssetsPanel.hpp"
#include "HyperPanels/SceneHierarchyPanel.hpp"

namespace HyperEditor
{
	class EditorLayer : public HyperLayer::OverlayLayer
	{
	private:
		/* Todo: Replace temporary shader handle with assets */
		HyperRendering::ShaderHandle m_SpriteShader{ 0 };

		AssetsPanel m_AssetsPanel;
		SceneHierarchyPanel m_SceneHierarchyPanel;

	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;

		virtual void OnEvent(HyperEvent::Event& event) override;

		virtual void OnUpdate(HyperUtilities::Timestep timeStep) override;
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
