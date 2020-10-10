#pragma once

#include "Core/Core.hpp"
#include "ECS/ECS.hpp"

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
		SceneHierarchyPanel();
		SceneHierarchyPanel(const Ref<Scene>& context);

		void OnRender();

		void SetContext(const Ref<Scene>& context);

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents();

		void DrawSelection();

		void DrawGlobalPopup();
		void DrawAddComponentPopup();
		void DrawRemoveComponentPopup();
	};
}
