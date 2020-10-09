#include "SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <limits>

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
		{
			m_SelectedEntity = -1;
			m_GlobalPopupOpen = false;
		}

		if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
		{
			m_GlobalPopupOpen = true;
		}

		if (m_GlobalPopupOpen)
			if (ImGui::IsWindowHovered())
				ImGui::OpenPopup("GlobalPopup");
			else
			{
				m_GlobalPopupOpen = false;
				ImGui::CloseCurrentPopup();
			}

		DrawGlobalPopup();

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

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, tag.c_str());

		if (m_SelectedEntity == entity)
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.5f;
			ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
			ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false);
		}

		if (ImGui::IsItemHovered())
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.5f;
			ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
			ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false);
		}

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		if (opened)
		{
			if (m_Context->GetRegistry().HasComponent<TransformComponent>(entity))
				ImGui::BulletText("Transform Component");
			if (m_Context->GetRegistry().HasComponent<SpriteRendererComponent>(entity))
				ImGui::BulletText("Sprite Renderer Component");
			if (m_Context->GetRegistry().HasComponent<CameraComponent>(entity))
				ImGui::BulletText("Camera Component");
			if (m_Context->GetRegistry().HasComponent<CameraControllerComponent>(entity))
				ImGui::BulletText("Camera Controller Component");
			if (m_Context->GetRegistry().HasComponent<CharacterControllerComponent>(entity))
				ImGui::BulletText("Character Controller Component");
			/*
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, tag.c_str());

			if (opened)
				ImGui::TreePop();
				*/
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

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}

		if (registry.HasComponent<TransformComponent>(entity))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
                TransformComponent& transformComponent = registry.GetComponent<TransformComponent>(entity);
				ImGui::NextColumn();
				ImGui::NextColumn();

				float width = ImGui::GetContentRegionAvailWidth() / 3;

				ImGui::Text("Position");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##XX", &transformComponent.Position.x, 0.01f, 0.0f, 0.0f, "X: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##XY", &transformComponent.Position.y, 0.01f, 0.0f, 0.0f, "Y: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##XZ", &transformComponent.Position.z, 0.01f, 0.0f, 0.0f, "Z: %.2f", 1.0f);
				ImGui::NextColumn();

				ImGui::Text("Rotation");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##YX", &transformComponent.Rotation.x, 0.01f, 0.0f, 0.0f, "X: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##YY", &transformComponent.Rotation.y, 0.01f, 0.0f, 0.0f, "Y: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##YZ", &transformComponent.Rotation.z, 0.01f, 0.0f, 0.0f, "Z: %.2f", 1.0f);
				ImGui::NextColumn();

				ImGui::Text("Scale");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##ZX", &transformComponent.Scale.x, 0.01f, 0.0f, 0.0f, "X: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##ZY", &transformComponent.Scale.y, 0.01f, 0.0f, 0.0f, "Y: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##ZZ", &transformComponent.Scale.z, 0.01f, 0.0f, 0.0f, "Z: %.2f", 1.0f);
				ImGui::NextColumn();

				ImGui::TreePop();
			}
			ImGui::Columns(1);
			ImGui::PopStyleVar();
		}

		if (registry.HasComponent<SpriteRendererComponent>(entity))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				SpriteRendererComponent& spriteRendererComponent = registry.GetComponent<SpriteRendererComponent>(entity);
				ImGui::NextColumn();
				ImGui::NextColumn();

				ImGui::Text("Color");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::ColorEdit4("##Color", spriteRendererComponent.Color);
				ImGui::NextColumn();

				ImGui::TreePop();
			}
			ImGui::Columns(1);
			ImGui::PopStyleVar();
		}

		if (registry.HasComponent<CameraComponent>(entity))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CameraComponent& cameraComponent = registry.GetComponent<CameraComponent>(entity);
				ImGui::NextColumn();
				ImGui::NextColumn();

				ImGui::Text("Width");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragInt("##Width", (int*)&cameraComponent.Width, 1.0f);
				ImGui::NextColumn();

				ImGui::Text("Height");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragInt("##Height", (int*)&cameraComponent.Height);
				ImGui::NextColumn();

				ImGui::Text("Zoom");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragFloat("##Zoom", &cameraComponent.Zoom, 0.1f, 0.1f, (std::numeric_limits<float>::max)());
				ImGui::NextColumn();

				ImGui::Text("Near Plane");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragFloat("##NearPlane", &cameraComponent.NearPlane);
				ImGui::NextColumn();

				ImGui::Text("Far Plane");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragFloat("##FarPlane", &cameraComponent.FarPlane);
				ImGui::NextColumn();

				ImGui::TreePop();
			}
			ImGui::Columns(1);
			ImGui::PopStyleVar();
		}

		if (registry.HasComponent<CameraControllerComponent>(entity))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(CameraControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CameraControllerComponent& cameraControllerComponent = registry.GetComponent<CameraControllerComponent>(entity);
				ImGui::NextColumn();
				ImGui::NextColumn();

				ImGui::Text("Move Speed");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragFloat("##MoveSpeedCamera", &cameraControllerComponent.MoveSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				ImGui::NextColumn();

				ImGui::Text("Zoom Speed");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragFloat("##ZoomSpeedCamera", &cameraControllerComponent.ZoomSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				ImGui::NextColumn();

				ImGui::TreePop();
			}
			ImGui::Columns(1);
			ImGui::PopStyleVar();
		}

		if (registry.HasComponent<CharacterControllerComponent>(entity))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(CharacterControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CharacterControllerComponent& characterControllerComponent = registry.GetComponent<CharacterControllerComponent>(entity);
				ImGui::NextColumn();
				ImGui::NextColumn();

				ImGui::Text("Speed");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				ImGui::DragFloat("##SpeedCharacter", &characterControllerComponent.Speed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				ImGui::NextColumn();

				ImGui::TreePop();
			}
			ImGui::Columns(1);
			ImGui::PopStyleVar();
		}

		ImGui::Columns(1);
		ImGui::Separator();
		if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)))
		{

		}
	}

	void SceneHierarchyPanel::DrawGlobalPopup()
	{
		if (ImGui::BeginPopup("GlobalPopup"))
		{
			if (ImGui::BeginMenu("2D Objects"))
			{
				if (ImGui::MenuItem("Quad"))
				{
					m_Context->CreateEntity();
				}
				if (ImGui::MenuItem("Triangle"))
				{

				}
				if (ImGui::MenuItem("Circle"))
				{

				}
				if (ImGui::MenuItem("Light"))
				{

				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("3D Objects"))
			{
				if (ImGui::MenuItem("Cube"))
				{

				}
				if (ImGui::MenuItem("Sphere"))
				{

				}
				if (ImGui::BeginMenu("Lights"))
				{
					if (ImGui::MenuItem("Spot Light"))
					{

					}
					if (ImGui::MenuItem("Directional Light"))
					{

					}
					if (ImGui::MenuItem("Point Light"))
					{

					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Cameras"))
			{
				if (ImGui::MenuItem("Orthographic Camera"))
				{

				}
				if (ImGui::MenuItem("Projection Camera"))
				{

				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}
