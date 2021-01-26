#pragma once

#include <Hyperion.hpp>

#include "Panels/SceneHierarchyPanel.hpp"

using namespace Hyperion;

class EditorLayer : public OverlayLayer
{
private:
	ShaderHandle m_SpriteShader = { 0 };
	Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;

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

	void DrawSelection();
};
