#pragma once

#include <HyperEngine.hpp>

namespace HyperEditor
{
	class SceneHierarchyPanel
	{
	private:
		HyperCore::Ref<HyperRendering::Scene> m_Scene{ nullptr };
		HyperECS::Entity m_SelectedEntity{ HyperECS::Null };

		bool m_SceneSelected{ false };
		bool m_AddComponentPopup{ false };
		bool m_RemoveComponentPopup{ false };

	public:
		SceneHierarchyPanel() = default;
		~SceneHierarchyPanel() = default;

		void OnRender();

		inline void SceneHierarchyPanel::SetScene(const HyperCore::Ref<HyperRendering::Scene>& scene)
		{
			m_Scene = scene;
		}

	private:
		void DrawEntityNode(HyperECS::Entity entity);
		void DrawSceneInformation();
		void DrawComponentInformation();

		template<class T>
		void DrawComponent(const std::string& componentName);

		void DrawDisplay(const std::string& name, rttr::variant value, HyperUtilities::ReflectionMeta meta);
		rttr::variant DrawEdit(const std::string& name, rttr::variant value, HyperUtilities::ReflectionMeta meta);

		template <class T, typename... Args>
		void DrawAddComponentMenu(Args&&... args);
		void DrawAddComponentPopup();

		template <class T>
		void DrawRemoveComponentMenu();
		void DrawRemoveComponentPopup();
	};
}
