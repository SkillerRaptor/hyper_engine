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

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNodeEx(m_Context->GetName().c_str(), flags))
		{
			m_Context->Each([&](Entity entity)
				{
					DrawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectedEntity = { 0, nullptr };
				m_GlobalPopupOpen = false;
			}

			if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
				m_GlobalPopupOpen = true;

			if (m_GlobalPopupOpen)
				if (ImGui::IsWindowHovered())
					ImGui::OpenPopup("GlobalPopup");
				else
				{
					m_GlobalPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}

			DrawGlobalPopup();

			ImGui::TreePop();
		}
		ImGui::End();

		ImGui::Begin("Inspector");
		if (m_SelectedEntity.GetEntityHandle() != 0)
			DrawComponents();
		ImGui::End();
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if ((uint32_t)m_SelectedEntity == (uint32_t)entity)
			DrawSelection();

		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("_ENTITY", &entity, sizeof(Entity));
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ENTITY"))
			{
				HP_CORE_ASSERT(payload->DataSize == sizeof(Entity), "Not an Entity");
				Entity entity = *(const Entity*)payload->Data;
				std::cout << entity.GetComponent<TagComponent>().Tag << std::endl;
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
		{
			//std::vector<uint32_t>::iterator it = std::find(m_Context->GetRegistry().GetEntities().begin(), m_Context->GetRegistry().GetEntities().end(), entity.GetEntityHandle());
			//std::vector<uint32_t>::iterator itCopy = it;
			//
			//ImGui::GetMouseDragDelta(0).y < 0.f ? ++itCopy : --itCopy;
			//if (itCopy != m_Context->GetRegistry().GetEntities().end())
			//{
			//	std::iter_swap(m_Context->GetRegistry().GetEntities().begin() + (it - m_Context->GetRegistry().GetEntities().begin()), m_Context->GetRegistry().GetEntities().begin() + (itCopy - m_Context->GetRegistry().GetEntities().begin()));
			//	ImGui::ResetMouseDragDelta();
			//}
		}

		if (opened)
		{
			if (entity.HasComponent<TransformComponent>())
				ImGui::BulletText("Transform Component");
			if (entity.HasComponent<SpriteRendererComponent>())
				ImGui::BulletText("Sprite Renderer Component");
			if (entity.HasComponent<CameraComponent>())
				ImGui::BulletText("Camera Component");
			if (entity.HasComponent<CameraControllerComponent>())
				ImGui::BulletText("Camera Controller Component");
			if (entity.HasComponent<CharacterControllerComponent>())
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

	void SceneHierarchyPanel::DrawComponents()
	{
		Registry& registry = m_Context->GetRegistry();

		ImGui::PushItemWidth(-1.0f);

		Entity& entity = m_SelectedEntity;
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - ImGui::GetContentRegionAvailWidth() / 4);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer) == std::string() ? "Empty!" : std::string(buffer);
			ImGui::SameLine();
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text] = ImVec4(0.502f, 0.502f, 0.502f, 1.00f);
			ImGui::Text("%u", entity);
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		}

		if (entity.HasComponent<TransformComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
				ImGui::NextColumn();
				ImGui::NextColumn();

				float width = ImGui::GetContentRegionAvailWidth() / 3;

				ImGui::Text("Position");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##XX", &transformComponent.Position.x, 0.1f, 0.0f, 0.0f, "X: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##XY", &transformComponent.Position.y, 0.1f, 0.0f, 0.0f, "Y: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##XZ", &transformComponent.Position.z, 0.1f, 0.0f, 0.0f, "Z: %.2f", 1.0f);
				ImGui::NextColumn();

				ImGui::Text("Rotation");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##YX", &transformComponent.Rotation.x, 1.0f, 0.0f, 360.0f, "X: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##YY", &transformComponent.Rotation.y, 1.0f, 0.0f, 360.0f, "Y: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##YZ", &transformComponent.Rotation.z, 1.0f, 0.0f, 360.0f, "Z: %.2f", 1.0f);
				ImGui::NextColumn();

				ImGui::Text("Scale");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##ZX", &transformComponent.Scale.x, 0.1f, 0.1f, (std::numeric_limits<float>::max)(), "X: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##ZY", &transformComponent.Scale.y, 0.1f, 0.1f, (std::numeric_limits<float>::max)(), "Y: %.2f", 1.0f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(width);
				ImGui::DragFloat("##ZZ", &transformComponent.Scale.z, 0.1f, 0.1f, (std::numeric_limits<float>::max)(), "Z: %.2f", 1.0f);
				ImGui::NextColumn();

				ImGui::TreePop();
			}
			ImGui::Columns(1);
			ImGui::PopStyleVar();
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				SpriteRendererComponent& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
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

		if (entity.HasComponent<CameraComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
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

		if (entity.HasComponent<CameraControllerComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(CameraControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera Controller"))
			{
				CameraControllerComponent& cameraControllerComponent = entity.GetComponent<CameraControllerComponent>();
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

		if (entity.HasComponent<CharacterControllerComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::Columns(2);
			ImGui::Separator();
			if (ImGui::TreeNodeEx((void*) typeid(CharacterControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Character Controller"))
			{
				CharacterControllerComponent& characterControllerComponent = entity.GetComponent<CharacterControllerComponent>();
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

		if (!ImGui::IsWindowHovered() && (m_AddComponentPopupOpen || m_RemoveComponentPopupOpen))
		{
			m_AddComponentPopupOpen = false;
			m_RemoveComponentPopupOpen = false;
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_AddComponentPopupOpen = false;
			m_RemoveComponentPopupOpen = false;
		}

		bool addComponent = false;
		bool removeComponent = false;

		if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>() ||
			!m_SelectedEntity.HasComponent<CameraComponent>() ||
			!m_SelectedEntity.HasComponent<CameraControllerComponent>() ||
			!m_SelectedEntity.HasComponent<CharacterControllerComponent>())
			addComponent = true;

		if (m_SelectedEntity.HasComponent<SpriteRendererComponent>() ||
			m_SelectedEntity.HasComponent<CameraComponent>() ||
			m_SelectedEntity.HasComponent<CameraControllerComponent>() ||
			m_SelectedEntity.HasComponent<CharacterControllerComponent>())
			removeComponent = true;

		if (addComponent)
		{
			if (ImGui::Button("Add Component", ImVec2(removeComponent ? ImGui::GetContentRegionAvailWidth() * 0.5f : ImGui::GetContentRegionAvailWidth(), 0.0f)))
				m_AddComponentPopupOpen = true;

			if (m_AddComponentPopupOpen)
				ImGui::OpenPopup("ComponentAddPopup");

			DrawAddComponentPopup();

			if (removeComponent) ImGui::SameLine();
		}

		if (removeComponent)
		{
			if (ImGui::Button("Remove Component", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)))
				m_RemoveComponentPopupOpen = true;

			if (m_RemoveComponentPopupOpen)
				ImGui::OpenPopup("ComponentRemovePopup");

			DrawRemoveComponentPopup();
		}
	}

	void SceneHierarchyPanel::DrawSelection()
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.x -= 5.0f;
		pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.5f;
		ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false, 5.0f);
	}

	void SceneHierarchyPanel::DrawGlobalPopup()
	{
		if (ImGui::BeginPopup("GlobalPopup"))
		{
			if (ImGui::BeginMenu("2D Objects"))
			{
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Quad"))
				{
					m_Context->CreateEntity();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Triangle"))
				{
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Circle"))
				{
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Light"))
				{
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				ImGui::EndMenu();
			}
			if (ImGui::IsItemHovered())
				DrawSelection();
			if (ImGui::BeginMenu("3D Objects"))
			{
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Cube"))
				{
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Sphere"))
				{
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::BeginMenu("Lights"))
				{
					if (ImGui::MenuItem("Spot Light"))
					{
					}
					if (ImGui::IsItemHovered())
						DrawSelection();

					if (ImGui::MenuItem("Directional Light"))
					{
					}
					if (ImGui::IsItemHovered())
						DrawSelection();

					if (ImGui::MenuItem("Point Light"))
					{
					}
					if (ImGui::IsItemHovered())
						DrawSelection();

					ImGui::EndMenu();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				ImGui::EndMenu();
			}
			if (ImGui::IsItemHovered())
				DrawSelection();

			if (ImGui::BeginMenu("Cameras"))
			{
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Orthographic Camera"))
				{
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				if (ImGui::MenuItem("Projection Camera"))
				{
				}
				if (ImGui::IsItemHovered())
					DrawSelection();

				ImGui::EndMenu();
			}
			if (ImGui::IsItemHovered())
				DrawSelection();

			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::DrawAddComponentPopup()
	{
		if (ImGui::BeginPopup("ComponentAddPopup"))
		{
			Registry& registry = m_Context->GetRegistry();
			if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer Component"))
				{
					m_SelectedEntity.AddComponent<SpriteRendererComponent>(Vec4(1.0f));
					m_AddComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}

			if (!m_SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera Component"))
				{
					m_SelectedEntity.AddComponent<CameraComponent>(1280, 720, 1.0f, 0.1f, 1.0f);
					m_AddComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}

			if (!m_SelectedEntity.HasComponent<CameraControllerComponent>())
			{
				if (ImGui::MenuItem("Camera Controller Component"))
				{
					m_SelectedEntity.AddComponent<CameraControllerComponent>(0.01f, 1.0f);
					m_AddComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}

			if (!m_SelectedEntity.HasComponent<CharacterControllerComponent>())
			{
				if (ImGui::MenuItem("Character Controller Component"))
				{
					m_SelectedEntity.AddComponent<CharacterControllerComponent>(1.0f);
					m_AddComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}
			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::DrawRemoveComponentPopup()
	{
		if (ImGui::BeginPopup("ComponentRemovePopup"))
		{
			Registry& registry = m_Context->GetRegistry();
			if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer Component"))
				{
					m_SelectedEntity.RemoveComponent<SpriteRendererComponent>();
					m_RemoveComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}

			if (m_SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera Component"))
				{
					m_SelectedEntity.RemoveComponent<CameraComponent>();
					m_RemoveComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}

			if (m_SelectedEntity.HasComponent<CameraControllerComponent>())
			{
				if (ImGui::MenuItem("Camera Controller Component"))
				{
					m_SelectedEntity.RemoveComponent<CameraControllerComponent>();
					m_RemoveComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}

			if (m_SelectedEntity.HasComponent<CharacterControllerComponent>())
			{
				if (ImGui::MenuItem("Character Controller Component"))
				{
					m_SelectedEntity.RemoveComponent<CharacterControllerComponent>();
					m_RemoveComponentPopupOpen = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered())
					DrawSelection();
			}
			ImGui::EndPopup();
		}
	}
}
