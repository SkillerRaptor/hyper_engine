#pragma once

#include <Hyperion.hpp>

#include "Panels/SceneHierarchyPanel.hpp"

using namespace Hyperion;

class EditorLayer : public OverlayLayer
{
private:
	ShaderHandle m_SpriteShader = 0;
	Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;

public:
	EditorLayer();
	virtual ~EditorLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEvent(Event& event) override;
	virtual void OnUpdate(Timestep timeStep) override;
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
