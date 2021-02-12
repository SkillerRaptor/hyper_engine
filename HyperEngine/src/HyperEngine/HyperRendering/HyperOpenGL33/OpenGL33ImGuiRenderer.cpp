#include "HyperRendering/HyperOpenGL33/OpenGL33ImGuiRenderer.hpp"

#include "HyperRendering/HyperImGui/ImGuiOpenGLRenderer.hpp"

namespace HyperRendering
{
	void OpenGL33ImGuiRenderer::OnAttach()
	{
		ImGui_ImplOpenGL3_Init("#version 330 core");
	}

	void OpenGL33ImGuiRenderer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	void OpenGL33ImGuiRenderer::Start()
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void OpenGL33ImGuiRenderer::End()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
