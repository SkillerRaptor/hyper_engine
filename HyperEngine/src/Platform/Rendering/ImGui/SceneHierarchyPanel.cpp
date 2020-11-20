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
				m_SelectedEntity = { 0, nullptr };

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
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.502f, 0.502f, 0.502f, 1.00f));
			ImGui::Text("%u", entity.GetEntityHandle());
			ImGui::PopStyleColor();
		}

		DrawComponent<TransformComponent>("Transform", [&](TransformComponent& component)
			{
				DrawDragVec3("Position", component.Position);
				DrawDragVec3("Rotation", component.Rotation, 0.1f, 0.0f, 360.0f);
				DrawDragVec3("Scale", component.Scale, 0.1f, 0.0f, (std::numeric_limits<float>::max)());

				if (component.Rotation.x < 0.0f) component.Rotation.x = 0.0f;
				if (component.Rotation.x > 360.0f) component.Rotation.x = 360.0f;
				if (component.Rotation.y < 0.0f) component.Rotation.y = 0.0f;
				if (component.Rotation.y > 360.0f) component.Rotation.y = 360.0f;
				if (component.Rotation.z < 0.0f) component.Rotation.z = 0.0f;
				if (component.Rotation.z > 360.0f) component.Rotation.z = 360.0f;

				if (component.Scale.x <= 0.0f) component.Scale.x = 0.1f;
				if (component.Scale.y <= 0.0f) component.Scale.y = 0.1f;
				if (component.Scale.z <= 0.0f) component.Scale.z = 0.1f;
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", [&](SpriteRendererComponent& component)
			{
				DrawColorEdit4("Sprite Color", component.Color);
			});

		DrawComponent<CameraComponent>("Camera", [&](CameraComponent& component)
			{
				DrawDragInt("Width", component.Width, 1);

				DrawDragInt("Height", component.Height, 1);

				DrawDragFloat("Zoom", component.Zoom, 0.1f, 0.1f, (std::numeric_limits<float>::max)());
				if (component.Zoom <= 0.0f) component.Zoom = 0.1f;

				DrawDragFloat("Near Plane", component.NearPlane);

				DrawDragFloat("Far Plane", component.FarPlane);

				DrawCheckbox("Primary", component.Primary);
			});

		DrawComponent<CameraControllerComponent>("Camera Controller", [&](CameraControllerComponent& component)
			{
				DrawDragFloat("Move Speed", component.MoveSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				if (component.MoveSpeed < 0.0f) component.MoveSpeed = 0.0f;

				DrawDragFloat("Zoom Speed", component.ZoomSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				if (component.ZoomSpeed < 0.0f) component.ZoomSpeed = 0.0f;
			});

		DrawComponent<CharacterControllerComponent>("Character Controller", [&](CharacterControllerComponent& component)
			{
				DrawDragFloat("Speed", component.Speed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
				if (component.Speed < 0.0f) component.Speed = 0.0f;
			});

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

		if (distance > 0 && distance < (uint64_t)entities.size())
			upComponent = true;

		if (distance >= 0 && distance < (uint64_t)(entities.size() - 1))
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

	template<class T>
	void SceneHierarchyPanel::DrawComponent(const std::string& componentName, typename std::common_type<std::function<void(T&)>>::type function)
	{
		if (m_SelectedEntity.HasComponent<T>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			if (ImGui::TreeNodeEx((void*) typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, componentName.c_str()))
			{
				function(m_SelectedEntity.GetComponent<T>());

				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
		}
	}

	void SceneHierarchyPanel::DrawAddComponentPopup()
	{
		if (ImGui::BeginPopup("ComponentAddPopup"))
		{
			Registry& registry = m_Context->GetRegistry();

			DrawAddComponentMenu<SpriteRendererComponent>(Vec4(1.0f));
			DrawAddComponentMenu<CameraComponent>(1280, 720, 5.0f, 0.1f, 1.0f, false, CameraComponent::CameraTypeInfo::ORTHOGRAPHIC);
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

	void SceneHierarchyPanel::DrawSelection()
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.x -= 5.0f;
		pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.5f;
		ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false, 5.0f);
	}

	void SceneHierarchyPanel::DrawCheckbox(const std::string& title, bool& value)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::Checkbox(std::string("##" + title).c_str(), &value);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawColorEdit4(const std::string& title, Vec4& value)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::ColorEdit4(std::string("##" + title).c_str(), value);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawDragInt(const std::string& title, uint32_t& value, int speed, int min, int max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), min, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawDragFloat(const std::string& title, float& value, float speed, float min, float max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::DragFloat(std::string("##" + title).c_str(), &value, speed, min, max, "%.2f", 1.0f);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawDragVec3(const std::string& title, Vec3& vector, float speed, float min, float max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		ImGui::DragFloat(std::string("##" + title + "X").c_str(), &vector.x, speed, min, max, "X: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "Y").c_str(), &vector.y, speed, min, max, "Y: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "Z").c_str(), &vector.z, speed, min, max, "Z: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	std::string SceneHierarchyPanel::SplitComponentName(const std::string& componentName)
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

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	const Ref<Scene>& SceneHierarchyPanel::GetContext() const
	{
		return m_Context;
	}
}
