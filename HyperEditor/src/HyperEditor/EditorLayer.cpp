#include "EditorLayer.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "HyperUtilities/FontAwesome.hpp"
#include "HyperUtilities/PanelUtilities.hpp"

namespace HyperEditor
{
	EditorLayer::EditorLayer()
		: OverlayLayer{ "Editor Layer" }, m_AssetsPanel{}, m_SceneHierarchyPanel{}
	{
	}

	void EditorLayer::OnAttach()
	{
		/* Setting variables*/
		AssetsManager::Init();
		PanelUtilities::SetRenderContext(m_RenderContext);
		m_AssetsPanel.SetTextureManager(m_RenderContext->GetTextureManager());
		m_SceneHierarchyPanel.SetScene(m_Scene);
		
		m_AssetsPanel.OnAttach();

		/* Adding Font */
		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig config{};
		config.MergeMode = true;
		static const ImWchar icon_ranges[] = { 0xf000, 0xf307, 0 };
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 14.0f, &config, icon_ranges);

		/* Loading Shaders */
		m_SpriteShader = m_RenderContext->GetShaderManager()->CreateShader("assets/shaders/SpriteShaderVertex.glsl", "assets/shaders/SpriteShaderFragment.glsl");
	
		m_Scene->CreateEntity("Test Entity");
	}

	void EditorLayer::OnEvent(HyperEvent::Event& event)
	{
		m_AssetsPanel.OnEvent(event);
	}

	void EditorLayer::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		m_AssetsPanel.OnUpdate();
	}

	void EditorLayer::OnRender()
	{
		CreateDockingMenu();

		m_RenderContext->GetTextureManager(),

		m_AssetsPanel.OnRender();
		m_SceneHierarchyPanel.OnRender();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));

		HyperCore::Ref<HyperRendering::SceneRecorder> sceneRecorder = m_RenderContext->GetSceneRecorder();

		ImGui::Begin(ICON_FK_WRENCH " Editor");
		ImGui::BeginChild("EditorRenderer");

		ImVec2 editorViewportPanelSize = ImGui::GetContentRegionAvail();

		sceneRecorder->SetEditorFocused(ImGui::IsWindowFocused() && ImGui::IsWindowHovered());
		sceneRecorder->RenderEditorImage();
		sceneRecorder->SetEditorViewportSize(glm::vec2{ editorViewportPanelSize.x, editorViewportPanelSize.y });

		ImGui::EndChild();
		ImGui::End();

		ImGui::Begin(ICON_FK_GAMEPAD " Game");
		ImGui::BeginChild("GameRenderer");

		ImVec2 gameViewportPanelSize = ImGui::GetContentRegionAvail();

		sceneRecorder->SetGameFocused(ImGui::IsWindowFocused() && ImGui::IsWindowHovered());
		sceneRecorder->RenderGameImage();
		sceneRecorder->SetGameViewportSize(glm::vec2{ gameViewportPanelSize.x, gameViewportPanelSize.y });

		ImGui::EndChild();
		ImGui::End();

		ImGui::PopStyleVar();
	}

	void EditorLayer::CreateDockingMenu()
	{
		static bool dockspaceOpen = true;

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspaceId = ImGui::GetID("DockSpace");

		if (!ImGui::DockBuilderGetNode(dockspaceId))
		{
			ImGui::DockBuilderRemoveNode(dockspaceId);
			ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_None);

			ImGuiID dockMainId = dockspaceId;

			ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.15f, nullptr, &dockMainId);
			ImGuiID dockLeftTopId = ImGui::DockBuilderSplitNode(dockLeftId, ImGuiDir_Up, 0.5f, nullptr, &dockLeftId);
			ImGuiID dockLeftDownId = ImGui::DockBuilderSplitNode(dockLeftId, ImGuiDir_Down, 0.5f, nullptr, &dockLeftId);

			ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.275f, nullptr, &dockMainId);

			ImGui::DockBuilderDockWindow(ICON_FK_LIST " Hierarchy", dockLeftTopId);
			ImGui::DockBuilderDockWindow(ICON_FK_INFO_CIRCLE " Inspector", dockRightId);
			ImGui::DockBuilderDockWindow(ICON_FK_FILES_O " Assets", dockLeftDownId);
			ImGui::DockBuilderDockWindow(ICON_FK_GAMEPAD " Game", dockMainId);
			ImGui::DockBuilderDockWindow(ICON_FK_WRENCH " Editor", dockMainId);

			ImGui::DockBuilderFinish(dockMainId);
		}

		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

		CreateMenuBar();

		ImGui::End();
	}

	void EditorLayer::CreateMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			CreateMenuFile();
			CreateMenuEdit();

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::CreateMenuFile()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl+N"))
				CreateNewScene();
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Open", "Ctrl+O"))
				OpenScene();
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Open Recent", ""))
				HP_CORE_WARN("'Open Recent' menu item is not implemented yet!");
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				SaveScene();
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
				SaveAsScene();
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			ImGui::Separator();

			if (ImGui::BeginMenu("Options"))
				ImGui::EndMenu();
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			ImGui::EndMenu();
		}
	}

	void EditorLayer::CreateMenuEdit()
	{
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z"))
				HP_CORE_WARN("'Undo' menu item is not implemented yet!");
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Redo", "Ctrl+Y"))
				HP_CORE_WARN("'Redo' menu item is not implemented yet!");
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			ImGui::Separator();

			if (ImGui::MenuItem("Cut", "Ctrl+X"))
				HP_CORE_WARN("'Cut' menu item is not implemented yet!");
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Copy", "Ctrl+C"))
				HP_CORE_WARN("'Copy' menu item is not implemented yet!");
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Paste", "Ctrl+V"))
				HP_CORE_WARN("'Paste' menu item is not implemented yet!");
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			if (ImGui::MenuItem("Delete", "Del"))
				HP_CORE_WARN("'Delete' menu item is not implemented yet!");
			if (ImGui::IsItemHovered())
				PanelUtilities::DrawSelection();

			ImGui::EndMenu();
		}
	}

	void EditorLayer::CreateNewScene()
	{
	}

	void EditorLayer::OpenScene()
	{
		std::optional<std::string> filePath = HyperUtilities::PlatformUtils::Get()->OpenFile("Hyper Scene (*.hyper)\0*.hyper\0");

		if (!filePath.has_value())
			return;

		m_Scene->Clear();
		m_Scene->SetName("Example Scene");

		HyperUtilities::SceneSerializer sceneSerializer{ m_Scene };
		sceneSerializer.Deserialize(filePath.value());
	}

	void EditorLayer::SaveScene()
	{
		HyperUtilities::SceneSerializer sceneSerializer{ m_Scene };
		std::string scenePath("assets/scenes/" + m_Scene->GetName() + ".hyper");

		std::replace(scenePath.begin(), scenePath.end(), ' ', '_');

		sceneSerializer.Serialize(scenePath);
	}

	void EditorLayer::SaveAsScene()
	{
		std::optional<std::string> filePath = HyperUtilities::PlatformUtils::Get()->SaveFile("Hyper Scene (*.hyper)\0*.hyper\0");

		if (!filePath.has_value())
			return;

		HyperUtilities::SceneSerializer sceneSerializer{ m_Scene };
		sceneSerializer.Serialize(filePath.value() + ".hyper");
	}
}
