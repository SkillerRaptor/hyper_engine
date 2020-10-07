#include "SceneHierarchyPanel.hpp"

#include <imgui.h>

#include "ECS/Components.hpp"

namespace Hyperion
{
	SceneHierarchyPanel::SceneHierarchyPanel()
	{
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
		: m_Context(context)
	{
	}

	void SceneHierarchyPanel::OnRender()
	{
		ImGui::Begin("Hierarchy");

		m_Context->GetRegistry().Each([&](uint32_t entity)
			{
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = -1;

		ImGui::End();

		ImGui::Begin("Inspector");
		if (m_SelectedEntity != -1)
			DrawComponents(m_SelectedEntity);
		ImGui::End();
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::DrawEntityNode(uint32_t entity)
	{
		auto& tag = m_Context->GetRegistry().GetComponent<TagComponent>(entity).Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t) entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, tag.c_str());

			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawComponents(uint32_t entity)
	{
		Registry& registry = m_Context->GetRegistry();

		ImGui::PushItemWidth(-1.0f);

		if (registry.HasComponent<TagComponent>(entity))
		{
			auto& tag = registry.GetComponent<TagComponent>(entity).Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			ImGui::Text("Tag");
			ImGui::SameLine();
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}

		if (registry.HasComponent<TransformComponent>(entity))
		{
			if (ImGui::TreeNodeEx((void*) typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				TransformComponent& transformComponent = registry.GetComponent<TransformComponent>(entity);

				ImGui::Text("Position");
				ImGui::SameLine();
				ImGui::DragFloat3("##X", transformComponent.Position, 0.1f);

				ImGui::Text("Rotation");
				ImGui::SameLine();
				ImGui::DragFloat3("##Y", transformComponent.Rotation, 0.1f);

				ImGui::Text("Scale   ");
				ImGui::SameLine();
				ImGui::DragFloat3("##Z", transformComponent.Scale, 0.1f);

				ImGui::TreePop();
			}
		}
	}
}
