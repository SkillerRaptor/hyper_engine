#pragma once

#include <Hyperion.hpp>

namespace Hyperion
{
	class SceneHierarchyPanel
	{
	private:
		Ref<Scene> m_Scene;
		bool m_AddComponentPopupOpen = false;
		bool m_RemoveComponentPopupOpen = false;
		HyperEntity m_SelectedEntity = { Entity(), nullptr };

	public:
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void OnRender();

		void SetScene(const Ref<Scene>& scene);
		const Ref<Scene>& GetScene() const;

		const HyperEntity& GetSelectedEntity() const;

	private:
		void DrawEntityNode(HyperEntity entity);
		void DrawComponents();

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
}
