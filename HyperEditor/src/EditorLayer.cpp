#include "EditorLayer.hpp"

#include <imgui.h>
#include <imgui_internal.h>

EditorLayer::EditorLayer()
	: OverlayLayer("Editor Layer")
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
	m_SpriteShader = m_RenderContext->GetShaderManager()->CreateShader("assets/shaders/SpriteShaderVertex.glsl", "assets/shaders/SpriteShaderFragment.glsl");

	for (size_t i = 0; i < 500; i++)
	{
		//std::stringstream ss;
		//ss << "Square-" << i;
		Hyperion::HyperEntity square = m_Scene->CreateEntity(std::string("Square-" + i));
		square.AddComponent<Hyperion::SpriteRendererComponent>(glm::vec4(Hyperion::Random::Float(0.0f, 1.0f), Hyperion::Random::Float(0.0f, 1.0f), Hyperion::Random::Float(0.0f, 1.0f), 1.0f));
		auto& transform = square.GetComponent<Hyperion::TransformComponent>();
		static constexpr int32_t RANGE = 50;
		transform.Position += glm::vec3{ 1.0f * Hyperion::Random::Int16(-RANGE, RANGE), 1.0f * Hyperion::Random::Int16(-RANGE, RANGE), 1.0f * Hyperion::Random::Int16(-RANGE, RANGE) };
	}

	m_SceneHierarchyPanel = Hyperion::CreateRef<Hyperion::SceneHierarchyPanel>(m_Scene);
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnEvent(Hyperion::Event& event)
{
}

void EditorLayer::OnUpdate(Hyperion::Timestep timeStep)
{
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

	Hyperion::ImGuiFrameSizeInfo& imGuiEditorSizeInfo = m_RenderContext->GetImGuiEditorSizeInfo();
	imGuiEditorSizeInfo.XPos = static_cast<uint32_t>(windowPos.x);
	imGuiEditorSizeInfo.YPos = static_cast<uint32_t>(windowPos.y);
	imGuiEditorSizeInfo.Width = static_cast<uint32_t>(windowSize.x);
	imGuiEditorSizeInfo.Height = static_cast<uint32_t>(windowSize.y);

	//m_EditorCamera->SetViewportSize(glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y));

	m_SceneRecorder->RenderImage();

	ImGui::EndChild();
	ImGui::End();

	ImGui::Begin("Game");
	ImGui::BeginChild("GameRenderer");

	Hyperion::ImGuiFrameSizeInfo& imGuiGameSizeInfo = m_RenderContext->GetImGuiGameSizeInfo();
	imGuiGameSizeInfo.XPos = static_cast<uint32_t>(ImGui::GetWindowPos().x);
	imGuiGameSizeInfo.YPos = static_cast<uint32_t>(ImGui::GetWindowPos().y);
	imGuiGameSizeInfo.Width = static_cast<uint32_t>(ImGui::GetWindowSize().x);
	imGuiGameSizeInfo.Height = static_cast<uint32_t>(ImGui::GetWindowSize().y);

	m_SceneRecorder->RenderImage();

	ImGui::EndChild();
	ImGui::End();

	ImGui::PopStyleVar();

	ImGui::Begin("Assets");
	ShowAssetsMenu();
	ImGui::End();
}

void EditorLayer::ShowDockingMenu()
{
	static bool alwaysOpen = true;

	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &alwaysOpen, window_flags);
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

void EditorLayer::ShowAssetsMenu()
{

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

	if (ImGui::MenuItem("Open Recent", ""))
		ImGui::EndMenu();
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
	if (ImGui::MenuItem("Undo", "Ctrl+Z"))
		ImGui::EndMenu();
	if (ImGui::IsItemHovered())
		DrawSelection();

	if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false))
		ImGui::EndMenu();
	if (ImGui::IsItemHovered())
		DrawSelection();

	ImGui::Separator();

	if (ImGui::MenuItem("Cut", "Ctrl+X"))
		ImGui::EndMenu();
	if (ImGui::IsItemHovered())
		DrawSelection();

	if (ImGui::MenuItem("Copy", "Ctrl+C"))
		ImGui::EndMenu();
	if (ImGui::IsItemHovered())
		DrawSelection();

	if (ImGui::MenuItem("Paste", "Ctrl+V"))
		ImGui::EndMenu();
	if (ImGui::IsItemHovered())
		DrawSelection();

	if (ImGui::MenuItem("Delete", "Del"))
		ImGui::EndMenu();
	if (ImGui::IsItemHovered())
		DrawSelection();

	ImGui::EndMenu();
}

void EditorLayer::NewScene()
{
}

void EditorLayer::OpenScene()
{
	std::string filePath = Hyperion::PlatformUtils::Get()->OpenFile("Hyper Scene (*.hyper)\0*.hyper\0");

	if (filePath.empty())
		return;

	//m_Scene = CreateRef<Scene>("Example Scene", m_RenderContext->GetRenderer2D());
	//m_SceneHierarchyPanel->SetContext(m_Scene);

	//SceneSerializer sceneSerializer(m_Scene);
	//sceneSerializer.Deserialize(filePath);
}

void EditorLayer::SaveScene()
{
	//SceneSerializer sceneSerializer(m_Scene);
	std::string scenePath("assets/scenes/" + m_Scene->GetName() + ".hyper");

	std::replace(scenePath.begin(), scenePath.end(), ' ', '_');

	//sceneSerializer.Serialize(scenePath);
}

void EditorLayer::SaveAsScene()
{
	std::string filePath = Hyperion::PlatformUtils::Get()->SaveFile("Hyper Scene (*.hyper)\0*.hyper\0");

	if (filePath.empty())
		return;

	//SceneSerializer sceneSerializer(m_Scene);
	//sceneSerializer.Serialize(filePath + ".hyper");
}

void EditorLayer::DrawSelection()
{
	ImVec2 pos = ImGui::GetCursorScreenPos();
	pos.x -= 5.0f;
	pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.5f;
	ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
	ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false, 5.0f);
}
