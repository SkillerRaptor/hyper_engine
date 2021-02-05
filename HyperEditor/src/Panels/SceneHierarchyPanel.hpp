#pragma once

#include <Hyperion.hpp>

using namespace Hyperion;

class SceneHierarchyPanel
{
private:
	Ref<Scene> m_Scene;
	Entity m_SelectedEntity;

	bool m_SceneSelected = false;
	bool m_AddComponentPopup = false;
	bool m_RemoveComponentPopup = false;

public:
	SceneHierarchyPanel(const Ref<Scene>& scene);

	void OnRender();

	void SetScene(const Ref<Scene>& scene);
	const Ref<Scene>& GetScene() const;

private:
	void DrawEntityNode(Entity entity);
	void DrawSceneInformation();
	void DrawComponentInformation();

	template<class T>
	void DrawComponent(const std::string& componentName);

	void DrawDisplay(const std::string& name, rttr::variant value, ReflectionMeta meta);
	rttr::variant DrawEdit(const std::string& name, rttr::variant value, ReflectionMeta meta);

	template <class T, typename... Args>
	void DrawAddComponentMenu(Args&&... args);
	void DrawAddComponentPopup();

	template <class T>
	void DrawRemoveComponentMenu();
	void DrawRemoveComponentPopup();
};
