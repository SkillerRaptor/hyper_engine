#include "OpenGL46ImGuiRenderer.hpp"

#include "Platform/Rendering/ImGui/ImGuiOpenGLRenderer.hpp"
#include "Platform/Rendering/OpenGL46/OpenGL46TextureManager.hpp"

namespace Hyperion
{
	OpenGL46ImGuiRenderer::OpenGL46ImGuiRenderer(Ref<RenderContext> renderContext)
		: ImGuiRenderer(renderContext)
	{
	}

	void OpenGL46ImGuiRenderer::OnAttach()
	{
		ImGui_ImplOpenGL3_Init("#version 440 core");
	}

	void OpenGL46ImGuiRenderer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	void OpenGL46ImGuiRenderer::OnUpdate(Timestep timeStep)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void OpenGL46ImGuiRenderer::OnRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void OpenGL46ImGuiRenderer::RenderImage()
	{
		ImGui::Image((ImTextureID)(intptr_t)m_BufferTextureId, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
	}

	void OpenGL46ImGuiRenderer::InitCapture()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_RenderContext->GetWindow()));

		Ref<TextureManager> textureManager = m_RenderContext->GetTextureManager();
		m_BufferTexture = textureManager->CreateTexture(data.Width, data.Height, TextureType::FRAMEBUFFER);
		m_BufferTextureId = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(m_BufferTexture))->TextureId;
	
		m_FrameBuffer = CreateScope<OpenGL46FrameBuffer>();
	}

	void OpenGL46ImGuiRenderer::StartCapture()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_RenderContext->GetWindow()));

		Ref<TextureManager> textureManager = m_RenderContext->GetTextureManager();
		OpenGLTextureData* textureData = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(m_BufferTexture));
		textureData->Width = data.Width;
		textureData->Height = data.Height;
		textureManager->DeleteTextureData(m_BufferTexture);
		textureManager->GenerateTexture(textureData, true);

		m_FrameBuffer->Bind();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_BufferTextureId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL46ImGuiRenderer::EndCapture()
	{
		m_FrameBuffer->Unbind();
	}
}
