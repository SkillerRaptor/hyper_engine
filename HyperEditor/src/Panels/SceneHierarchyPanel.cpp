#include "SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <limits>
#include <regex>
#include <sstream>

#include "Utilities/FontAwesome.hpp"
#include "Utilities/PanelUtilities.hpp"

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	: m_Scene(scene)
{
}

void SceneHierarchyPanel::OnRender()
{
	ImGui::Begin(ICON_FK_LIST " Hierarchy");
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	bool opened = ImGui::TreeNodeEx(m_Scene->GetName().c_str(), flags);

	if (m_SceneSelected)
		PanelUtilities::DrawSelection();

	if (ImGui::IsItemHovered())
		PanelUtilities::DrawSelection();

	if (ImGui::IsItemClicked())
	{
		m_SceneSelected = true;
		m_SelectedEntity.Invalidate();
	}

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		m_SelectedEntity.Invalidate();

	if (opened)
	{
		m_Scene->Each([&](HyperEntity entity)
			{
				DrawEntityNode(entity);
			});

		ImGui::TreePop();
	}
	ImGui::End();

	ImGui::Begin(ICON_FK_INFO_CIRCLE " Inspector");
	if (m_SceneSelected)
		DrawSceneInformation();
	else if (m_SelectedEntity.GetEntityHandle().IsHandleValid())
		DrawComponentInformation();
	ImGui::End();
}

void SceneHierarchyPanel::DrawSceneInformation()
{
	ImGui::PushItemWidth(-1.0f);
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	m_Scene->GetName().copy(buffer, sizeof(buffer));

	if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		m_Scene->SetName(std::string(buffer).empty() ? "Empty!" : std::string(buffer));
	ImGui::SameLine();
}

void SceneHierarchyPanel::DrawEntityNode(HyperEntity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(size_t)entity, flags, tag.c_str());

	if ((size_t)m_SelectedEntity == (size_t)entity)
		PanelUtilities::DrawSelection();

	if (ImGui::IsItemHovered())
		PanelUtilities::DrawSelection();

	if (ImGui::IsItemClicked())
	{
		m_SceneSelected = false;
		m_SelectedEntity = entity;
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
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawComponentInformation()
{
	World& registry = m_Scene->GetWorld();
	HyperEntity& entity = m_SelectedEntity;

	ImGui::PushItemWidth(-1.0f);
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		tag.copy(buffer, sizeof(buffer));

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - ImGui::GetContentRegionAvailWidth() / 4);
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			tag = std::string(buffer).empty() ? "Empty!" : std::string(buffer);
		ImGui::SameLine();
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.502f, 0.502f, 0.502f, 1.00f));
		ImGui::Text("%u", entity.GetEntityHandle());
		ImGui::PopStyleColor();
	}

	DrawComponent<TransformComponent>("Transform", [&](TransformComponent& component)
		{
			PanelUtilities::DrawDragVec3("Position", component.Position);
			PanelUtilities::DrawDragVec3("Rotation", component.Rotation, 0.1f, 0.0f, 360.0f);
			PanelUtilities::DrawDragVec3("Scale", component.Scale, 0.1f, 0.0f, (std::numeric_limits<float>::max)());

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
			PanelUtilities::DrawColorEdit4("Sprite Color", component.Color);
		});

	DrawComponent<CameraComponent>("Camera", [&](CameraComponent& component)
		{
			PanelUtilities::DrawDragUnsignedInt("Width", component.Width, 1);

			PanelUtilities::DrawDragUnsignedInt("Height", component.Height, 1);

			PanelUtilities::DrawDragFloat("Zoom", component.Zoom, 0.1f, 0.1f, (std::numeric_limits<float>::max)());
			if (component.Zoom <= 0.0f) component.Zoom = 0.1f;

			PanelUtilities::DrawDragFloat("Near Plane", component.NearPlane);

			PanelUtilities::DrawDragFloat("Far Plane", component.FarPlane);

			PanelUtilities::DrawCheckbox("Primary", component.Primary);
		});

	DrawComponent<CameraControllerComponent>("Camera Controller", [&](CameraControllerComponent& component)
		{
			PanelUtilities::DrawDragFloat("Move Speed", component.MoveSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
			if (component.MoveSpeed < 0.0f) component.MoveSpeed = 0.0f;

			PanelUtilities::DrawDragFloat("Zoom Speed", component.ZoomSpeed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
			if (component.ZoomSpeed < 0.0f) component.ZoomSpeed = 0.0f;
		});

	DrawComponent<CharacterControllerComponent>("Character Controller", [&](CharacterControllerComponent& component)
		{
			PanelUtilities::DrawDragFloat("Speed", component.Speed, 0.01f, 0.0f, (std::numeric_limits<float>::max)());
			if (component.Speed < 0.0f) component.Speed = 0.0f;
		});

	ImGui::Columns(1);
	ImGui::Separator();

	if (!ImGui::IsWindowHovered() && (m_AddComponentPopup || m_RemoveComponentPopup))
	{
		m_AddComponentPopup = false;
		m_RemoveComponentPopup = false;
	}

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
	{
		m_AddComponentPopup = false;
		m_RemoveComponentPopup = false;
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
			m_AddComponentPopup = true;

		if (m_AddComponentPopup)
			ImGui::OpenPopup("ComponentAddPopup");

		DrawAddComponentPopup();

		if (removeComponent) ImGui::SameLine();
	}

	if (removeComponent)
	{
		if (ImGui::Button("Remove Component", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)))
			m_RemoveComponentPopup = true;

		if (m_RemoveComponentPopup)
			ImGui::OpenPopup("ComponentRemovePopup");

		DrawRemoveComponentPopup();
	}
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
		World& world = m_Scene->GetWorld();

		DrawAddComponentMenu<SpriteRendererComponent>(glm::vec4{ 1.0f });
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
		World& registry = m_Scene->GetWorld();

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
			m_AddComponentPopup = false;
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::IsItemHovered())
			PanelUtilities::DrawSelection();
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
			m_RemoveComponentPopup = false;
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::IsItemHovered())
			PanelUtilities::DrawSelection();
	}
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

void SceneHierarchyPanel::SetScene(const Ref<Scene>& scene)
{
	m_Scene = scene;
}

const Ref<Scene>& SceneHierarchyPanel::GetScene() const
{
	return m_Scene;
}
