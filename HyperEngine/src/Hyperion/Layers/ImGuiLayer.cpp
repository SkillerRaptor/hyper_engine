#include "ImGuiLayer.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

#include "Core/Application.hpp"

namespace Hyperion
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui Layer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetNativeWindow()), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		SetupStyle();
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate(Timestep timeStep)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnRender(unsigned int frameTexture)
	{
		ShowDockingMenu();

		ImGui::Begin("Hierarchy");
		ImGui::PushTextWrapPos();
		ImGui::Text("Here you can see your files.");
		ImGui::PopTextWrapPos();
		ImGui::End();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(2, 2);

		ImGui::Begin("Editor");
		ImGui::BeginChild("GameRenderer");
		//Renderer2D::GetCamera()->SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		ImGui::Image((ImTextureID)(intptr_t)frameTexture, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();
		ImGui::End();

		style.WindowPadding = ImVec2(15, 15);

		ImGui::Begin("Console");
		ImGui::PushTextWrapPos();
		ImGui::Text("Here you can see your Console log.");
		ImGui::PopTextWrapPos();
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::PushTextWrapPos();
		ImGui::Text("Here you can see the components from an entity.");
		ImGui::PopTextWrapPos();
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::SetupStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowMenuButtonPosition = ImGuiDir_None;

		style.WindowPadding = ImVec2(15, 15);
		style.WindowRounding = 5.0f;
		style.FramePadding = ImVec2(5, 5);
		style.FrameRounding = 4.0f;
		style.ItemSpacing = ImVec2(12, 8);
		style.ItemInnerSpacing = ImVec2(8, 6);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 3.0f;

		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
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
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
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

	void ImGuiLayer::ShowDockingMenu()
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

			ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.2f, nullptr, &dockMainId);
			ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.2f, nullptr, &dockMainId);
			ImGuiID dockDownId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.2f, nullptr, &dockMainId);

			ImGui::DockBuilderDockWindow("Hierarchy", dockLeftId);
			ImGui::DockBuilderDockWindow("Inspector", dockRightId);
			ImGui::DockBuilderDockWindow("Console", dockDownId);
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

	void ImGuiLayer::ShowMenuFile()
	{
		if (ImGui::MenuItem("New", "Ctrl+N")) {}
		if (ImGui::MenuItem("Open", "Ctrl+O")) {}
		if (ImGui::MenuItem("Open Recent", "")) {}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {}
		if (ImGui::MenuItem("Save As..", "")) {}
		ImGui::Separator();
		if (ImGui::BeginMenu("Options"))
			ImGui::EndMenu();
		ImGui::EndMenu();
	}

	void ImGuiLayer::ShowMenuEdit()
	{
		if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
		if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}
		ImGui::Separator();
		if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
		if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
		if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
		if (ImGui::MenuItem("Delete", "Del")) {}
		ImGui::EndMenu();
	}
}