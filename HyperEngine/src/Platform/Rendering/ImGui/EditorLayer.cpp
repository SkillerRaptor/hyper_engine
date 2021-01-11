#include "EditorLayer.hpp"

#undef min
#undef max

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include "ImGuiGLFWRenderer.hpp"
#include "HyperCore/Application.hpp"
#include "HyperECS/Components.hpp"
#include "HyperECS/Scene/SceneSerializer.hpp"
#include "HyperEvents/KeyEvents.hpp"
#include "HyperUtilities/Input.hpp"
#include "HyperUtilities/PlatformUtils.hpp"

namespace Hyperion
{
	EditorLayer::EditorLayer(Ref<Scene> scene)
		: OverlayLayer("Editor Layer"), m_Scene(scene), m_Font(nullptr)
	{
	}

	void EditorLayer::OnAttach()
	{
		m_EditorCamera = CreateRef<EditorCamera>(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec2(1280.0f, 720.0f), 10.0f, 45.0f, 1.0f, 0.0f, 1000.0f, -90.0f, 0.0f, EditorCamera::CameraTypeInfo::PROJECTION);

		m_EditorCamera->m_ShaderManager = m_RenderContext->GetShaderManager();
		m_EditorCamera->UpdateProjectionMatrix();
		m_EditorCamera->UpdateViewMatrix();

		m_EditorRenderer = EditorRenderer::Construct(m_RenderContext);
		m_SceneHierarchyPanel = CreateRef<SceneHierarchyPanel>(m_Scene);

		m_EditorRenderer->OnAttach();
	}

	void EditorLayer::OnDetach()
	{
		m_EditorRenderer->OnDetach();
	}

	void EditorLayer::OnUpdate(Timestep timeStep)
	{
		m_EditorCamera->OnUpdate(timeStep);
		m_EditorRenderer->OnUpdate(timeStep);
	}

	void EditorLayer::OnRender()
	{
		ShowDockingMenu();

		m_SceneHierarchyPanel->OnRender();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));

		ImGui::Begin("Editor");
		ImGui::BeginChild("EditorRenderer");

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImGuiFrameSizeInfo& imGuiEditorSizeInfo = m_RenderContext->GetImGuiEditorSizeInfo();
		imGuiEditorSizeInfo.XPos = static_cast<uint32_t>(windowPos.x);
		imGuiEditorSizeInfo.YPos = static_cast<uint32_t>(windowPos.y);
		imGuiEditorSizeInfo.Width = static_cast<uint32_t>(windowSize.x);
		imGuiEditorSizeInfo.Height = static_cast<uint32_t>(windowSize.y);

		m_EditorCamera->SetViewportSize(glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y));

		m_EditorRenderer->RenderImage();

		HyperEntity selectedEntity = m_SceneHierarchyPanel->GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

			const glm::mat4& cameraProjection = m_EditorCamera->GetProjectionMatrix();
			const glm::mat4& cameraView = m_EditorCamera->GetViewMatrix();
		
			TransformComponent& transformComponent = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 rotation = glm::toMat4(glm::quat(transformComponent.Rotation));
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.Position) * rotation * glm::scale(glm::mat4(1.0f), transformComponent.Scale);

			bool snap = Input::IsKeyPressed(KeyCode::LeftControl);
			float snapValue = 0.5f;

			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(cameraView),
				glm::value_ptr(cameraProjection),
				static_cast<ImGuizmo::OPERATION>(m_GizmoType),
				ImGuizmo::MODE::LOCAL,
				glm::value_ptr(transform),
				nullptr,
				snap ? snapValues : nullptr);
		
			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));

				HP_CORE_DEBUG("-> %, %, %", glm::to_string(position), glm::to_string(rotation), glm::to_string(scale));

				glm::vec3 deltaRotation = rotation - transformComponent.Rotation;
				transformComponent.Position = position;
				transformComponent.Rotation += deltaRotation;
				transformComponent.Scale = scale;
			}
		}

		m_EditorCamera->m_Selected = ImGui::IsWindowFocused() && !ImGuizmo::IsUsing();

		ImGui::EndChild();
		ImGui::End();

		ImGui::Begin("Game");
		ImGui::BeginChild("GameRenderer");

		ImGuiFrameSizeInfo& imGuiGameSizeInfo = m_RenderContext->GetImGuiGameSizeInfo();
		imGuiGameSizeInfo.XPos = static_cast<uint32_t>(ImGui::GetWindowPos().x);
		imGuiGameSizeInfo.YPos = static_cast<uint32_t>(ImGui::GetWindowPos().y);
		imGuiGameSizeInfo.Width = static_cast<uint32_t>(ImGui::GetWindowSize().x);
		imGuiGameSizeInfo.Height = static_cast<uint32_t>(ImGui::GetWindowSize().y);

		m_EditorRenderer->RenderImage();

		ImGui::EndChild();
		ImGui::End();

		ImGui::PopStyleVar();

		ImGui::Begin("Assets");
		ShowAssetsMenu();
		ImGui::End();

		ImGui::PopFont();

		ImGui::Render();
		m_EditorRenderer->OnRender();
	}

	void EditorLayer::InitCapture()
	{
		m_EditorRenderer->InitCapture();
	}

	void EditorLayer::StartCapture()
	{
		m_EditorRenderer->StartCapture();
	}

	void EditorLayer::EndCapture()
	{
		m_EditorRenderer->EndCapture();
	}

	void EditorLayer::DrawSelection()
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.x -= 5.0f;
		pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.5f;
		ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false, 5.0f);
	}

	void EditorLayer::ShowAssetsMenu()
	{

	}

	void EditorLayer::ShowDockingMenu()
	{
		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		bool open = true;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &open, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspaceId = ImGui::GetID("DockSpace");

		if (!ImGui::DockBuilderGetNode(dockspaceId)) {

			ImGui::DockBuilderRemoveNode(dockspaceId);
			ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_None);

			ImGuiID dockMainId = dockspaceId;

			ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.15f, nullptr, &dockMainId);

			ImGuiID dockLeftTopId = ImGui::DockBuilderSplitNode(dockLeftId, ImGuiDir_Up, 0.5f, nullptr, &dockLeftId);
			ImGuiID dockLeftDownId = ImGui::DockBuilderSplitNode(dockLeftId, ImGuiDir_Down, 0.5f, nullptr, &dockLeftId);

			ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.275f, nullptr, &dockMainId);
			ImGuiID dockDownId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.2f, nullptr, &dockMainId);

			ImGui::DockBuilderDockWindow("Hierarchy", dockLeftTopId);
			ImGui::DockBuilderDockWindow("Inspector", dockRightId);
			ImGui::DockBuilderDockWindow("Assets", dockLeftDownId);
			ImGui::DockBuilderDockWindow("Game", dockMainId);
			ImGui::DockBuilderDockWindow("Editor", dockMainId);

			ImGui::DockBuilderFinish(dockMainId);
		}

		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
				ShowMenuFile();

			if (ImGui::BeginMenu("Edit"))
				ShowMenuEdit();

			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

	void EditorLayer::ShowMenuFile()
	{
		if (ImGui::MenuItem("New", "Ctrl+N"))
			NewScene();
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Open", "Ctrl+O"))
			OpenScene();
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Open Recent", "")) {}
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Save", "Ctrl+S"))
			SaveScene();
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
			SaveAsScene();
		if (ImGui::IsItemHovered())
			DrawSelection();

		ImGui::Separator();

		if (ImGui::BeginMenu("Options"))
			ImGui::EndMenu();
		if (ImGui::IsItemHovered())
			DrawSelection();

		ImGui::EndMenu();
	}

	void EditorLayer::ShowMenuEdit()
	{
		if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}
		if (ImGui::IsItemHovered())
			DrawSelection();

		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
		if (ImGui::IsItemHovered())
			DrawSelection();

		if (ImGui::MenuItem("Delete", "Del")) {}
		if (ImGui::IsItemHovered())
			DrawSelection();
		ImGui::EndMenu();
	}

	void EditorLayer::NewScene()
	{
	}

	void EditorLayer::OpenScene()
	{
		std::string filePath = PlatformUtils::Get()->OpenFile("Hyper Scene (*.hyper)\0*.hyper\0");
		if (!filePath.empty())
		{
			m_Scene = CreateRef<Scene>("Example Scene", m_RenderContext->GetRenderer2D());
			m_SceneHierarchyPanel->SetContext(m_Scene);

			SceneSerializer sceneSerializer(m_Scene);
			sceneSerializer.Deserialize(filePath);
		}
	}

	void EditorLayer::SaveScene()
	{
		SceneSerializer sceneSerializer(m_Scene);
		std::string scenePath("assets/scenes/" + m_Scene->GetName() + ".hyper");
		for (std::string::iterator it = scenePath.begin(); it != scenePath.end(); ++it)
			if (*it == ' ')
				*it = '_';
		sceneSerializer.Serialize(scenePath);
	}

	void EditorLayer::SaveAsScene()
	{
		std::string filePath = PlatformUtils::Get()->SaveFile("Hyper Scene (*.hyper)\0*.hyper\0");
		if (!filePath.empty())
		{
			SceneSerializer sceneSerializer(m_Scene);
			sceneSerializer.Serialize(filePath + ".hyper");
		}
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent& event)
			{
				if (event.GetRepeatCount() > 0)
					return false;

				bool controlPressed = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);
				bool shiftPressed = Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift);

				switch (event.GetKeyCode())
				{
				case KeyCode::N:
				{
					if (controlPressed)
						NewScene();
					break;
				}
				case KeyCode::O:
				{
					if (controlPressed)
						OpenScene();
					break;
				}
				case KeyCode::S:
				{
					if (controlPressed)
					{
						if (shiftPressed)
						{
							SaveAsScene();
							break;
						}
						SaveScene();
					}
					break;
				}
				case KeyCode::Q:
					m_GizmoType = -1;
					break;
				case KeyCode::T:
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					break;
				case KeyCode::R:
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
					break;
				case KeyCode::Z:
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
					break;
				default:
					break;
				}
				return true;
			});

		m_EditorCamera->OnEvent(event);
	}
}
