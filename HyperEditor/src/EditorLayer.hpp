#pragma once

#include <Hyperion.hpp>

#include "Panels/AssetsPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"

using namespace Hyperion;

class EditorLayer : public OverlayLayer
{
private:
	ShaderHandle m_SpriteShader = { 0 }; // FIXME: TEMP

	Scope<AssetsPanel> m_AssetsPanel;
	Scope<SceneHierarchyPanel> m_SceneHierarchyPanel;

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
