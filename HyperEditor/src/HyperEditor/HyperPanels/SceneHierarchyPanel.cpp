#include "HyperPanels/SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <limits>
#include <regex>
#include <sstream>

#include "HyperUtilities/FontAwesome.hpp"
#include "HyperUtilities/PanelUtilities.hpp"

namespace HyperEditor
{
	SceneHierarchyPanel::SceneHierarchyPanel(const HyperCore::Ref<HyperRendering::Scene>& scene)
		: m_Scene{ scene }
	{
	}

	void SceneHierarchyPanel::OnRender()
	{
		ImGui::Begin(ICON_FK_LIST " Hierarchy");
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
		bool sceneOpen = ImGui::TreeNodeEx(m_Scene->GetName().c_str(), flags);

		if (m_SceneSelected)
			PanelUtilities::DrawSelection();

		if (ImGui::IsItemHovered())
			PanelUtilities::DrawSelection();

		if (ImGui::IsItemClicked())
		{
			m_SceneSelected = true;
			m_SelectedEntity = HyperECS::Null;
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = HyperECS::Null;

		if (sceneOpen)
		{
			for (HyperECS::Entity entity : m_Scene->GetRegistry().GetEntities())
				DrawEntityNode(entity);

			ImGui::TreePop();
		}

		ImGui::End();

		ImGui::Begin(ICON_FK_INFO_CIRCLE " Inspector");
		if (m_SceneSelected)
			DrawSceneInformation();
		else if (m_SelectedEntity != HyperECS::Null)
			DrawComponentInformation();
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawSceneInformation()
	{
		ImGui::PushItemWidth(-1.0f);

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		m_Scene->GetName().copy(buffer, sizeof(buffer));

		if (ImGui::InputText("##SceneName", buffer, sizeof(buffer)))
			m_Scene->SetName(std::string(buffer).empty() ? "Empty!" : std::string(buffer));
	}

	void SceneHierarchyPanel::DrawEntityNode(HyperECS::Entity entity)
	{
		const std::string& tag = m_Scene->GetRegistry().GetComponent<HyperECS::TagComponent>(entity).GetTag();

		std::stringstream ss;
		ss << ICON_FK_CUBE << " " << tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(size_t)entity, flags, ss.str().c_str());

		if ((size_t)m_SelectedEntity == (size_t)entity)
			PanelUtilities::DrawSelection();

		if (ImGui::IsItemHovered())
			PanelUtilities::DrawSelection();

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_SceneSelected = false;
			m_SelectedEntity = entity;
		}
		else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{

		}

		if (opened)
			ImGui::TreePop();
	}

	void SceneHierarchyPanel::DrawComponentInformation()
	{
		HyperECS::Registry& registry = m_Scene->GetRegistry();

		ImGui::PushItemWidth(-1.0f);

		if (registry.HasComponent<HyperECS::TagComponent>(m_SelectedEntity))
		{
			std::string tag = registry.GetComponent<HyperECS::TagComponent>(m_SelectedEntity).GetTag();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			tag.copy(buffer, sizeof(buffer));

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - ImGui::GetContentRegionAvailWidth() / 4);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer).empty() ? "Empty!" : std::string(buffer);
			registry.GetComponent<HyperECS::TagComponent>(m_SelectedEntity).SetTag(tag);
			ImGui::SameLine();
			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.602f, 0.602f, 0.602f, 1.00f));
			ImGui::Text("%u", m_SelectedEntity);
			ImGui::PopStyleColor();
		}

		DrawComponent<HyperECS::TransformComponent>("Transform");
		DrawComponent<HyperECS::SpriteRendererComponent>("Sprite Renderer");
		DrawComponent<HyperECS::Rigidbody2DComponent>("Rigidbody2D");
		DrawComponent<HyperECS::CameraComponent>("Camera");

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

		if (!registry.HasComponent<HyperECS::SpriteRendererComponent>(m_SelectedEntity) ||
			!registry.HasComponent<HyperECS::CameraComponent>(m_SelectedEntity) ||
			!registry.HasComponent<HyperECS::Rigidbody2DComponent>(m_SelectedEntity))
			addComponent = true;

		if (registry.HasComponent<HyperECS::SpriteRendererComponent>(m_SelectedEntity) ||
			registry.HasComponent<HyperECS::CameraComponent>(m_SelectedEntity) ||
			registry.HasComponent<HyperECS::Rigidbody2DComponent>(m_SelectedEntity))
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
	void SceneHierarchyPanel::DrawComponent(const std::string& componentName)
	{
		HyperECS::Registry& registry = m_Scene->GetRegistry();
		if (registry.HasComponent<T>(m_SelectedEntity))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, componentName.c_str()))
			{
				T& component = registry.GetComponent<T>(m_SelectedEntity);
				rttr::instance componentInstance = component;
				rttr::type type = rttr::type::get<T>();

				HyperUtilities::ReflectionMeta typeMeta(type);
				for (auto& property : type.get_properties())
				{
					HyperUtilities::ReflectionMeta propertyMeta(property);
					if ((propertyMeta.Flags & HyperUtilities::MetaInfo::EDITABLE) == 0) continue;
					if (propertyMeta.Editor.ViewCondition != nullptr && !propertyMeta.Editor.ViewCondition(componentInstance)) continue;

					if (propertyMeta.Editor.CustomView != nullptr)
					{
						propertyMeta.Editor.CustomView(componentInstance);
						continue;
					}

					const char* name = property.get_name().cbegin();
					if (property.is_readonly())
					{
						DrawDisplay(name, property.get_value(componentInstance), propertyMeta);
					}
					else
					{
						rttr::variant editedValue = DrawEdit(name, property.get_value(componentInstance), propertyMeta);
						if (editedValue.is_valid())
							property.set_value(componentInstance, editedValue);
					}
				}

				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
		}
	}

	void SceneHierarchyPanel::DrawDisplay(const std::string& name, rttr::variant value, HyperUtilities::ReflectionMeta meta)
	{
	}

	rttr::variant SceneHierarchyPanel::DrawEdit(const std::string& name, rttr::variant value, HyperUtilities::ReflectionMeta meta)
	{
		rttr::type type = value.get_type();
		std::string typeName = type.get_name().cbegin();

		HyperUtilities::Range editRange = meta.Editor.EditRange;
		float editPrecision = meta.Editor.EditPrecision;

		switch (meta.Editor.InterpretAs)
		{
		case HyperUtilities::InterpretAsInfo::DEFAULT:
			if (type.is_enumeration())
			{
				return PanelUtilities::DrawCombo(name, value);
			}
			else if (typeName == "bool")
			{
				return PanelUtilities::DrawCheckbox(name, value.convert<bool>());
			}
			else if (typeName == "float")
			{
				return PanelUtilities::DrawDragFloat(name, value.convert<float>(), editPrecision, editRange.Min, editRange.Max);
			}
			else if (typeName == "Vector2")
			{
				return PanelUtilities::DrawDragVec2(name, value.convert<glm::vec2>(), editPrecision, editRange.Min, editRange.Max);
			}
			else if (typeName == "Vector3")
			{
				return PanelUtilities::DrawDragVec3(name, value.convert<glm::vec3>(), editPrecision, editRange.Min, editRange.Max);
			}
			else if (typeName == "Vector4")
			{
				return PanelUtilities::DrawDragVec4(name, value.convert<glm::vec4>(), editPrecision, editRange.Min, editRange.Max);
			}
			else if (typeName == "Texture")
			{
				return PanelUtilities::DrawSelectableImage(name, value.convert<HyperRendering::TextureHandle>());
			}
			break;
		case HyperUtilities::InterpretAsInfo::COLOR:
			if (typeName == "Vector3")
			{
				return PanelUtilities::DrawColorEdit3(name, value.convert<glm::vec3>());
			}
			else if (typeName == "Vector4")
			{
				return PanelUtilities::DrawColorEdit4(name, value.convert<glm::vec4>());
			}
			break;
		default:
			break;
		}
		return rttr::variant{};
	}

	template <class T, typename... Args>
	void SceneHierarchyPanel::DrawAddComponentMenu(Args&&... args)
	{
		HyperECS::Registry& registry = m_Scene->GetRegistry();
		if (!registry.HasComponent<T>(m_SelectedEntity))
		{
			rttr::type type = rttr::type::get<T>();
			if (ImGui::MenuItem(type.get_name().cbegin()))
			{
				registry.AddComponent<T>(m_SelectedEntity, std::forward<Args>(args)...);
				m_AddComponentPopup = false;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();
		}
	}

	void SceneHierarchyPanel::DrawAddComponentPopup()
	{
		if (ImGui::BeginPopup("ComponentAddPopup"))
		{
			DrawAddComponentMenu<HyperECS::SpriteRendererComponent>(glm::vec4{ 1.0f }, HyperRendering::TextureHandle{ 0 });
			DrawAddComponentMenu<HyperECS::CameraComponent>(glm::vec4{ 0.75f, 0.75f, 0.75f, 1.0f }, HyperECS::CameraComponent::ProjectionType::PERSPECTIVE, 90.0f, glm::vec2{ 0.1f, 1000.0f }, glm::vec2{ 1.0f, 1.0f }, false);
			DrawAddComponentMenu<HyperECS::Rigidbody2DComponent>(1.0f);

			ImGui::EndPopup();
		}
	}

	template <class T>
	void SceneHierarchyPanel::DrawRemoveComponentMenu()
	{
		HyperECS::Registry& registry = m_Scene->GetRegistry();
		if (registry.HasComponent<T>(m_SelectedEntity))
		{
			rttr::type type = rttr::type::get<T>();
			if (ImGui::MenuItem(type.get_name().cbegin()))
			{
				registry.RemoveComponent<T>(m_SelectedEntity);
				m_RemoveComponentPopup = false;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();
		}
	}

	void SceneHierarchyPanel::DrawRemoveComponentPopup()
	{
		if (ImGui::BeginPopup("ComponentRemovePopup"))
		{
			DrawRemoveComponentMenu<HyperECS::SpriteRendererComponent>();
			DrawRemoveComponentMenu<HyperECS::CameraComponent>();
			DrawRemoveComponentMenu<HyperECS::Rigidbody2DComponent>();

			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::SetScene(const HyperCore::Ref<HyperRendering::Scene>& scene)
	{
		m_Scene = scene;
	}

	const HyperCore::Ref<HyperRendering::Scene>& SceneHierarchyPanel::GetScene() const
	{
		return m_Scene;
	}
}
