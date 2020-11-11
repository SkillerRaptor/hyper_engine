#include "SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <limits>
#include <regex>
#include <sstream>

#include "HyperECS/Components.hpp"

namespace Hyperion
{
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
			ImGui::TreePop();
		}
	}

	static void DrawCheckbox(const std::string& title, bool& value)
	{
		std::stringstream ss;
		ss << "##" << title;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::Checkbox(ss.str().c_str(), &value);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawDragColorEdit4(const std::string& title, Vec4& value)
	{
		std::stringstream ss;
		ss << "##" << title;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::ColorEdit4(ss.str().c_str(), value);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawDragInt(const std::string& title, uint32_t& value, int speed = 1, int min = 0, int max = 0)
	{
		std::stringstream ss;
		ss << "##" << title;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::DragInt(ss.str().c_str(), (int*)&value, static_cast<float>(speed), min, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawDragFloat(const std::string& title, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f)
	{
		std::stringstream ss;
		ss << "##" << title;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::DragFloat(ss.str().c_str(), &value, speed, min, max, "%.2f", 1.0f);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawDragVec3(const std::string& title, Vec3& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		ImGui::DragFloat("##PositionX", &vector.x, speed, min, max, "X: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat("##PositionY", &vector.y, speed, min, max, "Y: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat("##PositionZ", &vector.z, speed, min, max, "Z: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawComponents()
	{
		Registry& registry = m_Context->GetRegistry();

		Entity& entity = m_SelectedEntity;

		ImGui::PushItemWidth(-1.0f);
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
			ImGui::Text("%u", entity.GetEntityHandle());
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		}

		if (entity.HasComponent<TransformComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			if (ImGui::TreeNodeEx((void*) typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();

				DrawDragVec3("Position", transformComponent.Position);
				DrawDragVec3("Rotation", transformComponent.Rotation, 0.1f, 0.0f, 360.0f);
				DrawDragVec3("Scale", transformComponent.Scale, 0.1f, 0.0f, (std::numeric_limits<float>::max)());

				if (transformComponent.Rotation.x < 0.0f) transformComponent.Rotation.x = 0.0f;
				if (transformComponent.Rotation.x > 360.0f) transformComponent.Rotation.x = 360.0f;
				if (transformComponent.Rotation.y < 0.0f) transformComponent.Rotation.y = 0.0f;
				if (transformComponent.Rotation.y > 360.0f) transformComponent.Rotation.y = 360.0f;
				if (transformComponent.Rotation.z < 0.0f) transformComponent.Rotation.z = 0.0f;
				if (transformComponent.Rotation.z > 360.0f) transformComponent.Rotation.z = 360.0f;

				if (transformComponent.Scale.x <= 0.0f) transformComponent.Scale.x = 0.1f;
				if (transformComponent.Scale.y <= 0.0f) transformComponent.Scale.y = 0.1f;
				if (transformComponent.Scale.z <= 0.0f) transformComponent.Scale.z = 0.1f;

				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			if (ImGui::TreeNodeEx((void*) typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				SpriteRendererComponent& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();

				DrawDragColorEdit4("Sprite Color", spriteRendererComponent.Color);

				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
		}

		if (entity.HasComponent<CameraComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			if (ImGui::TreeNodeEx((void*) typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();

				DrawDragInt("Width", cameraComponent.Width, 1);

				DrawDragInt("Height", cameraComponent.Height, 1);

				DrawDragFloat("Zoom", cameraComponent.Zoom, 0.1f, 0.1f, (std::numeric_limits<float>::max)());
				if (cameraComponent.Zoom <= 0.0f) cameraComponent.Zoom = 0.1f;

				DrawDragFloat("Near Plane", cameraComponent.NearPlane);

				DrawDragFloat("Far Plane", cameraComponent.FarPlane);

				DrawCheckbox("Primary", cameraComponent.Primary);

				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
		}

		if (entity.HasComponent<CameraControllerComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			if (ImGui::TreeNodeEx((void*) typeid(CameraControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera Controller"))
			{
				CameraControllerComponent& cameraControllerComponent = entity.GetComponent<CameraControllerComponent>();

				DrawDragFloat("Move Speed", cameraControllerComponent.MoveSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				if (cameraControllerComponent.MoveSpeed < 0.0f) cameraControllerComponent.MoveSpeed = 0.0f;

				DrawDragFloat("Zoom Speed", cameraControllerComponent.ZoomSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				if (cameraControllerComponent.ZoomSpeed < 0.0f) cameraControllerComponent.ZoomSpeed = 0.0f;

				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
		}

		if (entity.HasComponent<CharacterControllerComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			if (ImGui::TreeNodeEx((void*) typeid(CharacterControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Character Controller"))
			{
				CharacterControllerComponent& characterControllerComponent = entity.GetComponent<CharacterControllerComponent>();

				DrawDragFloat("Speed", characterControllerComponent.Speed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				if (characterControllerComponent.Speed < 0.0f) characterControllerComponent.Speed = 0.0f;

				ImGui::TreePop();
			}
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

		bool upComponent = false;
		bool downComponent = false;

		std::vector<uint32_t>& entities = m_Context->GetRegistry().GetEntities();
		std::vector<uint32_t>::iterator position = std::find(entities.begin(), entities.end(), entity.GetEntityHandle());
		uint64_t distance = std::abs(std::distance(entities.begin(), position));

		if (distance > 0 && distance < (uint64_t) entities.size())
			upComponent = true;

		if (distance >= 0 && distance < (uint64_t) (entities.size() - 1))
			downComponent = true;

		if (upComponent)
		{
			if (ImGui::Button("Move Entity Up", ImVec2(downComponent ? ImGui::GetContentRegionAvailWidth() * 0.5f : ImGui::GetContentRegionAvailWidth(), 0.0f)))
				std::iter_swap(position, position - 1);
			if (downComponent) ImGui::SameLine();
		}

		if (downComponent)
			if (ImGui::Button("Move Entity Down", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)))
				std::iter_swap(position, position + 1);
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

			DrawAddComponentMenu<SpriteRendererComponent>(Vec4(1.0f));
			DrawAddComponentMenu<CameraComponent>(1280, 720, 5.0f, 0.1f, 1.0f, false);
			DrawAddComponentMenu<CameraControllerComponent>(0.1f, 1.0f);
			DrawAddComponentMenu<CharacterControllerComponent>(1.0f);

			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::DrawRemoveComponentPopup()
	{
		if (ImGui::BeginPopup("ComponentRemovePopup"))
		{
			Registry& registry = m_Context->GetRegistry();

			DrawRemoveComponentMenu<SpriteRendererComponent>();
			DrawRemoveComponentMenu<CameraComponent>();
			DrawRemoveComponentMenu<CameraControllerComponent>();
			DrawRemoveComponentMenu<CharacterControllerComponent>();

			ImGui::EndPopup();
		}
	}

	template <class T, typename... Args>
	void SceneHierarchyPanel::DrawAddComponentMenu(Args&&... args)
	{
		if (!m_SelectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(SplitComponentName(typeid(T).name()).c_str()))
			{
				m_SelectedEntity.AddComponent<T>(std::forward<Args>(args)...);
				m_AddComponentPopupOpen = false;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsItemHovered())
				DrawSelection();
		}
	}

	template <class T>
	void SceneHierarchyPanel::DrawRemoveComponentMenu()
	{
		if (m_SelectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(SplitComponentName(typeid(T).name()).c_str()))
			{
				m_SelectedEntity.RemoveComponent<T>();
				m_RemoveComponentPopupOpen = false;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsItemHovered())
				DrawSelection();
		}
	}

	std::string Hyperion::SceneHierarchyPanel::SplitComponentName(const std::string& componentName)
	{
		std::string shortName = componentName.substr(componentName.find("::") + 2, componentName.length());
		std::regex regex("[A-Z][^A-Z]*");
		std::stringstream ss;

		auto regexBegin = std::sregex_iterator(shortName.begin(), shortName.end(), regex);
		auto regexEnd = std::sregex_iterator();

		for (std::sregex_iterator it = regexBegin; it != regexEnd; ++it)
			ss << it->str();

		return ss.str();
	}

	void SceneHierarchyPanel::DrawSelection()
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.x -= 5.0f;
		pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.5f;
		ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false, 5.0f);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	const Ref<Scene>& SceneHierarchyPanel::GetContext() const
	{
		return m_Context;
	}
}
