#pragma once

#include <Hyperion.hpp>

#include "Panel.hpp"

using namespace Hyperion;

class SceneHierarchyPanel : public Panel
{
private:
	Ref<Scene> m_Scene;
	HyperEntity m_SelectedEntity;

	bool m_SceneSelected = false;
	bool m_AddComponentPopup = false;
	bool m_RemoveComponentPopup = false;

public:
	SceneHierarchyPanel(const Ref<Scene>& scene);

	void OnRender();

	void SetScene(const Ref<Scene>& scene);
	const Ref<Scene>& GetScene() const;

private:
	void DrawEntityNode(HyperEntity entity);
	void DrawSceneInformation();
	void DrawComponentInformation();

	template<class T>
	void DrawComponent(const std::string& componentName, const typename std::common_type<std::function<void(T&)>>::type function);

	void DrawAddComponentPopup();
	void DrawRemoveComponentPopup();

	template <class T, typename... Args>
	void DrawAddComponentMenu(Args&&... args);

	template <class T>
	void DrawRemoveComponentMenu();

	std::string SplitComponentName(const std::string& componentName);
};
