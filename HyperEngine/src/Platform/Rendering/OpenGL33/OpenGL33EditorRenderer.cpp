#include "OpenGL33EditorRenderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Platform/Rendering/ImGui/ImGuiOpenGLRenderer.hpp"
#include "Platform/Rendering/OpenGL33/OpenGL33TextureManager.hpp"

namespace Hyperion
{
	OpenGL33EditorRenderer::OpenGL33EditorRenderer(Ref<RenderContext> renderContext)
		: EditorRenderer(renderContext), m_BufferTexture({ -1 }), m_BufferTextureId(-1)
	{
	}

	void OpenGL33EditorRenderer::OnAttach()
	{
		ImGui_ImplOpenGL3_Init("#version 330 core");
	}

	void OpenGL33EditorRenderer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	void OpenGL33EditorRenderer::OnUpdate(Timestep timeStep)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void OpenGL33EditorRenderer::OnRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void OpenGL33EditorRenderer::RenderImage()
	{
		ImGui::Image((ImTextureID)(intptr_t)m_BufferTextureId, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}

	void OpenGL33EditorRenderer::InitCapture()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_RenderContext->GetWindow()));

		Ref<TextureManager> textureManager = m_RenderContext->GetTextureManager();
		m_BufferTexture = textureManager->CreateTexture(data.Width, data.Height, TextureType::FRAMEBUFFER);
		m_BufferTextureId = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(m_BufferTexture))->TextureId;

		m_FrameBuffer = CreateScope<OpenGL33FrameBuffer>();
	}

	void OpenGL33EditorRenderer::StartCapture()
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

	void OpenGL33EditorRenderer::EndCapture()
	{
		m_FrameBuffer->Unbind();
	}
}
