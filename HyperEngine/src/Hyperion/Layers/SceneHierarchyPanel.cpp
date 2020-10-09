#include "SceneHierarchyPanel.hpp"

#include <imgui.h>

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
			m_ItemPopupOpen = false;
		}

		if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			m_GlobalPopupOpen = true;
			if (m_ItemPopupOpen) m_ItemPopupOpen = false;
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

		if (ImGui::IsMouseDown(1) && ImGui::IsAnyItemHovered() && m_SelectedEntity != -1)
		{
			m_ItemPopupOpen = true;
			if (m_GlobalPopupOpen) m_GlobalPopupOpen = false;
		}

		if (m_ItemPopupOpen)
			if (m_SelectedEntity != -1)
				ImGui::OpenPopup("ItemPopup");
			else
			{
				m_ItemPopupOpen = false;
				ImGui::CloseCurrentPopup();
			}

		DrawItemPopup();

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
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, tag.c_str());

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
				ImGui::DragFloat3("##X", transformComponent.Position, 0.01f);

				ImGui::Text("Rotation");
				ImGui::SameLine();
				ImGui::DragFloat3("##Y", transformComponent.Rotation, 0.01f);

				ImGui::Text("Scale   ");
				ImGui::SameLine();
				ImGui::DragFloat3("##Z", transformComponent.Scale, 0.01f);

				ImGui::TreePop();
			}
		}

		if (registry.HasComponent<SpriteRendererComponent>(entity))
		{
			if (ImGui::TreeNodeEx((void*) typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				SpriteRendererComponent& spriteRendererComponent = registry.GetComponent<SpriteRendererComponent>(entity);

				ImGui::Text("Color");
				ImGui::SameLine();
				ImGui::ColorEdit4("##Color", spriteRendererComponent.Color);

				ImGui::TreePop();
			}
		}

		if (registry.HasComponent<CameraComponent>(entity))
		{
			if (ImGui::TreeNodeEx((void*) typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CameraComponent& cameraComponent = registry.GetComponent<CameraComponent>(entity);

				ImGui::Text("Width     ");
				ImGui::SameLine();
				ImGui::DragInt("##Width", (int*)&cameraComponent.Width, 1.0f);

				ImGui::Text("Height    ");
				ImGui::SameLine();
				ImGui::DragInt("##Height", (int*)&cameraComponent.Height);

				ImGui::Text("Zoom      ");
				ImGui::SameLine();
				ImGui::DragFloat("##Zoom", &cameraComponent.Zoom, 0.1f, 0.1f, (std::numeric_limits<float>::max)());

				ImGui::Text("Near Plane");
				ImGui::SameLine();
				ImGui::DragFloat("##NearPlane", &cameraComponent.NearPlane);

				ImGui::Text("Far Plane ");
				ImGui::SameLine();
				ImGui::DragFloat("##FarPlane", &cameraComponent.FarPlane);

				ImGui::TreePop();
			}
		}

		if (registry.HasComponent<CameraControllerComponent>(entity))
		{
			if (ImGui::TreeNodeEx((void*) typeid(CameraControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CameraControllerComponent& cameraControllerComponent = registry.GetComponent<CameraControllerComponent>(entity);

				ImGui::Text("Move Speed");
				ImGui::SameLine();
				ImGui::DragFloat("##MoveSpeedCamera", &cameraControllerComponent.MoveSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());

				ImGui::Text("Zoom Speed");
				ImGui::SameLine();
				ImGui::DragFloat("##ZoomSpeedCamera", &cameraControllerComponent.ZoomSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());

				ImGui::TreePop();
			}
		}

		if (registry.HasComponent<CharacterControllerComponent>(entity))
		{
			if (ImGui::TreeNodeEx((void*) typeid(CharacterControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CharacterControllerComponent& characterControllerComponent = registry.GetComponent<CharacterControllerComponent>(entity);

				ImGui::Text("Speed");
				ImGui::SameLine();
				ImGui::DragFloat("##SpeedCharacter", &characterControllerComponent.Speed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());

				ImGui::TreePop();
			}
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

	void SceneHierarchyPanel::DrawItemPopup()
	{
		if (ImGui::BeginPopup("ItemPopup"))
		{
			if (ImGui::BeginMenu("Add Component"))
			{
				if (!m_Context->GetRegistry().HasComponent<TransformComponent>(m_SelectedEntity))
				{
					if (ImGui::MenuItem("Transform Component"))
					{
						m_Context->GetRegistry().AddComponent<TransformComponent>(m_SelectedEntity, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(10.0f, 10.0f, 10.0f));
						m_ItemPopupOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}

				if (!m_Context->GetRegistry().HasComponent<SpriteRendererComponent>(m_SelectedEntity))
				{
					if (ImGui::MenuItem("Sprite Renderer Component"))
					{
						m_Context->GetRegistry().AddComponent<SpriteRendererComponent>(m_SelectedEntity, Vec4(1.0f));
						m_ItemPopupOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}

				/*
				if (!m_Context->GetRegistry().HasComponent<CameraComponent>(m_SelectedEntity))
				{
					if (ImGui::MenuItem("Camera Component"))
					{
						m_Context->GetRegistry().AddComponent<CameraComponent>(m_SelectedEntity, 1280, 720, 1.0f, 0.1f, 1.0f);
						m_ItemPopupOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}

				if (!m_Context->GetRegistry().HasComponent<CameraControllerComponent>(m_SelectedEntity))
				{
					if (ImGui::MenuItem("Camera Controller Component"))
					{
						m_Context->GetRegistry().AddComponent<CameraControllerComponent>(m_SelectedEntity, 0.01f, 1.0f);
						m_ItemPopupOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}

				if (!m_Context->GetRegistry().HasComponent<CharacterControllerComponent>(m_SelectedEntity))
				{
					if (ImGui::MenuItem("Character Controller Component"))
					{
						m_Context->GetRegistry().AddComponent<CharacterControllerComponent>(m_SelectedEntity, 1.0f);
						m_ItemPopupOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}
				*/
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}
