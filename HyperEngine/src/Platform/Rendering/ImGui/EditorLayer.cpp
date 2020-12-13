#include "EditorLayer.hpp"

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
		: OverlayLayer("Editor Layer"), m_Scene(scene), m_EditorCamera(Vec3(0.0f), Vec2(1280.0f, 720.0f), 10.0f, 1.0f, 1.0f, -1.0f, 1.0f, 45.0f, 0.0f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		m_EditorCamera.m_ShaderManager = m_RenderContext->GetShaderManager();
		m_EditorCamera.UpdateProjectionMatrix();
		m_EditorCamera.UpdateViewMatrix();

		m_EditorRenderer = EditorRenderer::Construct(m_RenderContext);
		m_SceneHierarchyPanel = CreateRef<SceneHierarchyPanel>(m_Scene);

		HyperEntity squareOne = m_Scene->CreateEntity("Square One");
		HyperEntity squareTwo = m_Scene->CreateEntity("Square Two");
		HyperEntity squareThree = m_Scene->CreateEntity("Square Three");

		squareOne.AddComponent<SpriteRendererComponent>(Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		squareTwo.AddComponent<SpriteRendererComponent>(Vec4(0.0f, 1.0f, 0.0f, 1.0f));
		squareThree.AddComponent<SpriteRendererComponent>(Vec4(0.0f, 0.0f, 1.0f, 1.0f));

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		m_Font = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 15.0f);
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Application::Get()->GetNativeWindow()->GetWindow()), true);
		m_EditorRenderer->OnAttach();

		SetupStyle();
	}

	void EditorLayer::OnDetach()
	{
		m_EditorRenderer->OnDetach();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorLayer::OnUpdate(Timestep timeStep)
	{
		m_EditorCamera.OnUpdate(timeStep);
		m_EditorRenderer->OnUpdate(timeStep);

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::BeginFrame();

		ImGui::PushFont(m_Font);
	}

	void EditorLayer::OnRender()
	{
		ShowDockingMenu();

		m_SceneHierarchyPanel->OnRender();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));

		ImGui::Begin("Editor");
		ImGui::BeginChild("EditorRenderer");

		ImGuiFrameSizeInfo& imGuiEditorSizeInfo = m_RenderContext->GetImGuiEditorSizeInfo();
		imGuiEditorSizeInfo.XPos = static_cast<uint32_t>(ImGui::GetWindowPos().x);
		imGuiEditorSizeInfo.YPos = static_cast<uint32_t>(ImGui::GetWindowPos().y);
		imGuiEditorSizeInfo.Width = static_cast<uint32_t>(ImGui::GetWindowSize().x);
		imGuiEditorSizeInfo.Height = static_cast<uint32_t>(ImGui::GetWindowSize().y);

		m_EditorCamera.SetViewportSize(Vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y));

		m_EditorRenderer->RenderImage();

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

	void EditorLayer::SetupStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowMenuButtonPosition = ImGuiDir_None;

		style.WindowPadding = ImVec2(15, 15);
		style.WindowRounding = 5.0f;
		style.FramePadding = ImVec2(6, 3);
		style.FrameRounding = 2.0f;
		style.ItemSpacing = ImVec2(6, 6);
		style.ItemInnerSpacing = ImVec2(6, 3);
		style.IndentSpacing = 12.0f;
		style.ScrollbarSize = 10.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 4.0f;
		style.GrabRounding = 3.0f;

		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
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
				default:
					break;
				}
				return true;
			});

		m_EditorCamera.OnEvent(event);
	}
}
