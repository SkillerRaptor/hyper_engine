#include "OpenGL33ImGuiRenderer.hpp"

#include "Platform/Rendering/ImGui/ImGuiOpenGLRenderer.hpp"
#include "Platform/Rendering/OpenGL33/OpenGL33TextureManager.hpp"

namespace Hyperion
{
	OpenGL33ImGuiRenderer::OpenGL33ImGuiRenderer(Ref<RenderContext> renderContext)
		: ImGuiRenderer(renderContext)
	{
	}

	void OpenGL33ImGuiRenderer::OnAttach()
	{
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void OpenGL33ImGuiRenderer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	void OpenGL33ImGuiRenderer::OnUpdate(Timestep timeStep)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void OpenGL33ImGuiRenderer::OnRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void OpenGL33ImGuiRenderer::RenderImage()
	{
		ImGui::Image((ImTextureID)(intptr_t)m_BufferTextureId, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
	}

	void OpenGL33ImGuiRenderer::InitCapture()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_RenderContext->GetWindow()));

		Ref<TextureManager> textureManager = m_RenderContext->GetTextureManager();
		m_BufferTexture = textureManager->CreateTexture(data.Width, data.Height, TextureType::FRAMEBUFFER);
		m_BufferTextureId = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(m_BufferTexture))->TextureId;
	
		m_FrameBuffer = CreateScope<OpenGL33FrameBuffer>();
	}

	void OpenGL33ImGuiRenderer::StartCapture()
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

	void OpenGL33ImGuiRenderer::EndCapture()
	{
		m_FrameBuffer->Unbind();
	}
}
