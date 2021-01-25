#include "OpenGL33SceneRecorder.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "Platform/Rendering/OpenGL33/OpenGL33TextureManager.hpp"

namespace Hyperion
{
	OpenGL33SceneRecorder::OpenGL33SceneRecorder(Ref<RenderContext> renderContext)
		: SceneRecorder(renderContext)
	{
	}

	static uint32_t colorRenderBuffer;
	static uint32_t depthRenderBuffer;

	static TextureHandle depthTexture;
	static uint32_t depthTextureId;

	void OpenGL33SceneRecorder::InitRecording()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_RenderContext->GetWindow()));
		Ref<TextureManager> textureManager = m_RenderContext->GetTextureManager();

		m_FrameBuffer = CreateScope<OpenGL33FrameBuffer>(data.Width, data.Height);
		m_FrameBuffer->Bind();

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		m_BufferTexture = textureManager->CreateTexture(data.Width, data.Height, TextureType::FRAMEBUFFER);
		m_BufferTextureId = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(m_BufferTexture))->TextureId;

		depthTexture = textureManager->CreateTexture(data.Width, data.Height, TextureType::DEPTH);
		depthTextureId = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(depthTexture))->TextureId;

		glGenRenderbuffers(1, &colorRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, data.Width, data.Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);

		glGenRenderbuffers(1, &depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, data.Width, data.Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

		HP_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is Incomplete");
		m_FrameBuffer->Unbind();
	}

	void OpenGL33SceneRecorder::StartRecording()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_RenderContext->GetWindow()));
		Ref<TextureManager> textureManager = m_RenderContext->GetTextureManager();

		m_FrameBuffer->Bind();
		glEnable(GL_DEPTH_TEST);

		auto* textureData = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(m_BufferTexture));
		textureData->Width = data.Width;
		textureData->Height = data.Height;
		textureManager->DeleteTextureData(m_BufferTexture);
		textureManager->GenerateTexture(textureData, nullptr);

		textureData = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(depthTexture));
		textureData->Width = data.Width;
		textureData->Height = data.Height;
		textureManager->DeleteTextureData(depthTexture);
		textureManager->GenerateTexture(textureData, nullptr);

		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL33SceneRecorder::EndRecording()
	{
		m_FrameBuffer->Unbind();
	}

	void OpenGL33SceneRecorder::RenderImage()
	{
		ImGui::Image((ImTextureID)(intptr_t)m_BufferTextureId, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}	
}
