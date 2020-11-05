#pragma once

#include "Scene.hpp"
#include "Core/Core.hpp"
#include "HyperECS/Entity.hpp"

namespace Hyperion
{
	class SceneHierarchyPanel
	{
	private:
		Ref<Scene> m_Context;
		bool m_GlobalPopupOpen = false;
		bool m_AddComponentPopupOpen = false;
		bool m_RemoveComponentPopupOpen = false;
		Entity m_SelectedEntity = { 0, nullptr };

	public:
		SceneHierarchyPanel(const Ref<Scene>& context);

		void OnRender();

		void SetContext(const Ref<Scene>& context);
		const Ref<Scene>& GetContext() const;

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents();

		void DrawGlobalPopup();
		void DrawAddComponentPopup();
		void DrawRemoveComponentPopup();

		template <class T, typename... Args>
		void DrawAddComponentMenu(Args&&... args);

		template <class T>
		void DrawRemoveComponentMenu();

		std::string SplitComponentName(const std::string& componentName);

		void DrawSelection();
	};
}
