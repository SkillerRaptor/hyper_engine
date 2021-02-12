#include "HyperRendering/HyperOpenGL46/OpenGL46ImGuiRenderer.hpp"

#include "HyperRendering/HyperImGui/ImGuiOpenGLRenderer.hpp"

namespace HyperRendering
{
	void OpenGL46ImGuiRenderer::OnAttach()
	{
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}

	void OpenGL46ImGuiRenderer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	void OpenGL46ImGuiRenderer::Start()
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void OpenGL46ImGuiRenderer::End()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
