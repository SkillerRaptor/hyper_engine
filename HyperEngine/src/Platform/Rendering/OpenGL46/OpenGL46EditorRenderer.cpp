#include "OpenGL46EditorRenderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Platform/Rendering/ImGui/ImGuiOpenGLRenderer.hpp"
#include "Platform/Rendering/OpenGL46/OpenGL46TextureManager.hpp"

namespace Hyperion
{
	OpenGL46EditorRenderer::OpenGL46EditorRenderer(Ref<RenderContext> renderContext)
		: EditorRenderer(renderContext)
	{
	}

	void OpenGL46EditorRenderer::OnAttach()
	{
		ImGui_ImplOpenGL3_Init("#version 440 core");
	}

	void OpenGL46EditorRenderer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	void OpenGL46EditorRenderer::OnUpdate(Timestep timeStep)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void OpenGL46EditorRenderer::OnRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void OpenGL46EditorRenderer::RenderImage()
	{
		ImGui::Image((ImTextureID)(intptr_t)m_BufferTextureId, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
	}

	void OpenGL46EditorRenderer::InitCapture()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_RenderContext->GetWindow()));

		Ref<TextureManager> textureManager = m_RenderContext->GetTextureManager();
		m_BufferTexture = textureManager->CreateTexture(data.Width, data.Height, TextureType::FRAMEBUFFER);
		m_BufferTextureId = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(m_BufferTexture))->TextureId;
	
		m_FrameBuffer = CreateScope<OpenGL46FrameBuffer>();
	}

	void OpenGL46EditorRenderer::StartCapture()
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

	void OpenGL46EditorRenderer::EndCapture()
	{
		m_FrameBuffer->Unbind();
	}
}
