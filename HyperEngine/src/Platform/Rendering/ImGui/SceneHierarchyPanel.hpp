#pragma once

#include "HyperCore/Core.hpp"
#include "HyperECS/Entity.hpp"
#include "HyperECS/Scene/Scene.hpp"

namespace Hyperion
{
	class SceneHierarchyPanel
	{
	private:
		Ref<Scene> m_Context;
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

		template<class T>
		void DrawComponent(const std::string& componentName, const typename std::common_type<std::function<void(T&)>>::type function);

		void DrawAddComponentPopup();
		void DrawRemoveComponentPopup();

		template <class T, typename... Args>
		void DrawAddComponentMenu(Args&&... args);

		template <class T>
		void DrawRemoveComponentMenu();

		void DrawCheckbox(const std::string& title, bool& value);
		void DrawColorEdit4(const std::string& title, Vec4& value);
		void DrawDragInt(const std::string& title, uint32_t& value, int speed = 1, int min = 0, int max = 0);
		void DrawDragFloat(const std::string& title, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
		void DrawDragVec3(const std::string& title, Vec3& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

		void DrawSelection();

		std::string SplitComponentName(const std::string& componentName);
	};
}
