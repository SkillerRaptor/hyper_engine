#pragma once

#include <Hyperion.hpp>

#include "Panels/SceneHierarchyPanel.hpp"

class EditorLayer : public Hyperion::OverlayLayer
{
private:
	Hyperion::ShaderHandle m_SpriteShader;
	Hyperion::Ref<Hyperion::SceneHierarchyPanel> m_SceneHierarchyPanel;

public:
	EditorLayer();
	virtual ~EditorLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEvent(Hyperion::Event& event) override;
	virtual void OnUpdate(Hyperion::Timestep timeStep) override;
	virtual void OnRender() override;

private:
	void ShowDockingMenu();

	void ShowAssetsMenu();

	void ShowMenuFile();
	void ShowMenuEdit();

	void NewScene();
	void OpenScene();
	void SaveScene();
	void SaveAsScene();

	void DrawSelection();
};
