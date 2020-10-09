#pragma once

#include "Core/Core.hpp"
#include "ECS/Scene.hpp"

namespace Hyperion
{
	class SceneHierarchyPanel
	{
	private:
		Ref<Scene> m_Context;
		bool m_GlobalPopupOpen = false;
		bool m_ComponentPopupOpen = false;
		uint32_t m_SelectedEntity = -1;

	public:
		SceneHierarchyPanel();
		SceneHierarchyPanel(const Ref<Scene>& context);

		void OnRender();

		void SetContext(const Ref<Scene>& context);

	private:
		void DrawEntityNode(uint32_t entity);
		void DrawComponents(uint32_t entity);

		void DrawSelection();

		void DrawGlobalPopup();
		void DrawComponentPopup();
	};
}
