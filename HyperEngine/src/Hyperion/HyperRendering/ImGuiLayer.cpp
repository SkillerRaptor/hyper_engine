#include "ImGuiLayer.hpp"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <ImGuizmo.h>

#include "HyperCore/Application.hpp"

namespace Hyperion
{
	ImGuiLayer::ImGuiLayer()
		: OverlayLayer("ImGui Layer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		m_ImGuiRenderer = ImGuiRenderer::Construct(m_RenderContext);
		m_EditorCamera = CreateRef<EditorCamera>(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec2(1280.0f, 720.0f), 25.0f, 45.0f, 1.0f, 0.1f, 1000.0f, -90.0f, 0.0f, EditorCamera::CameraTypeInfo::PROJECTION);
		//m_EditorCamera = CreateRef<EditorCamera>(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec2(1280.0f, 720.0f), 0.1f, 45.0f, 1.0f, 0.0f, 1000.0f, -90.0f, 0.0f, EditorCamera::CameraTypeInfo::ORTHOGRAPHIC);

		m_EditorCamera->m_ShaderManager = m_RenderContext->GetShaderManager();
		m_EditorCamera->UpdateProjectionMatrix();
		m_EditorCamera->UpdateViewMatrix();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 15.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 15.0f);

		ImGui::StyleColorsDark();

		SetupStyle();

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		m_ImGuiRenderer->OnAttach();
	}

	void ImGuiLayer::OnDetach()
	{
		m_ImGuiRenderer->OnDetach();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);
	}

	void ImGuiLayer::OnUpdate(Timestep timeStep)
	{
		m_EditorCamera->OnUpdate(timeStep);
	}

	void ImGuiLayer::OnRender()
	{
		m_EditorCamera->SetViewportSize(glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y));
	}

	void ImGuiLayer::Start()
	{
		m_ImGuiRenderer->Start();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application* app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app->GetWindow()->GetWidth()), static_cast<float>(app->GetWindow()->GetHeight()));

		ImGui::Render();
		m_ImGuiRenderer->End();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetupStyle()
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
}
