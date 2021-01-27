#include "OpenGL33SceneRecorder.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "Platform/Rendering/OpenGL33/OpenGL33TextureManager.hpp"

namespace Hyperion
{
	OpenGL33SceneRecorder::OpenGL33SceneRecorder(Ref<TextureManager> textureManager, GLFWwindow* window)
		: SceneRecorder(textureManager, window)
	{
	}

	void OpenGL33SceneRecorder::InitRecorder()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_Window));

		m_GameFrameBuffer = CreateScope<OpenGL33FrameBuffer>(data.Width, data.Height);
		m_EditorFrameBuffer = CreateScope<OpenGL33FrameBuffer>(data.Width, data.Height);

		m_GameFrameBuffer->Bind();
		glEnable(GL_DEPTH_TEST);
		m_GameFrameBuffer->Unbind();

		m_EditorFrameBuffer->Bind();
		glEnable(GL_DEPTH_TEST);
		m_EditorFrameBuffer->Unbind();
	}

	void OpenGL33SceneRecorder::RebuildRecoder()
	{
		if (m_RebuildGameImage)
		{
			m_GameFrameBuffer->Resize(static_cast<uint32_t>(m_GameViewportSize.x), static_cast<uint32_t>(m_GameViewportSize.y));
			m_RebuildGameImage = false;
		}

		if (m_RebuildEditorImage)
		{
			m_EditorFrameBuffer->Resize(static_cast<uint32_t>(m_EditorViewportSize.x), static_cast<uint32_t>(m_EditorViewportSize.y));
			m_RebuildEditorImage = false;
		}
	}

	void OpenGL33SceneRecorder::StartGameRecording()
	{
		m_GameFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL33SceneRecorder::EndGameRecording()
	{
		m_GameFrameBuffer->Unbind();
	}

	void OpenGL33SceneRecorder::RenderGameImage()
	{
		uint32_t colorTextureId = static_cast<OpenGLTextureData*>(m_TextureManager->GetTextureData(m_GameFrameBuffer->GetColorAttachment()))->TextureId;
		ImGui::Image((ImTextureID)(intptr_t)colorTextureId, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}

	void OpenGL33SceneRecorder::StartEditorRecording()
	{
		m_EditorFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL33SceneRecorder::EndEditorRecording()
	{
		m_EditorFrameBuffer->Unbind();
	}

	void OpenGL33SceneRecorder::RenderEditorImage()
	{
		static uint32_t colorTextureId = static_cast<OpenGLTextureData*>(m_TextureManager->GetTextureData(m_EditorFrameBuffer->GetColorAttachment()))->TextureId;
		ImGui::Image((ImTextureID)(intptr_t)colorTextureId, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}

	TextureHandle OpenGL33SceneRecorder::GetGameColorAttachment()
	{
		return m_GameFrameBuffer ? m_GameFrameBuffer->GetColorAttachment() : TextureHandle{ 0 };
	}

	TextureHandle OpenGL33SceneRecorder::GetGameDepthAttachment()
	{
		return m_GameFrameBuffer ? m_GameFrameBuffer->GetDepthAttachment() : TextureHandle{ 0 };
	}

	TextureHandle OpenGL33SceneRecorder::GetEditorColorAttachment()
	{
		return m_EditorFrameBuffer ? m_EditorFrameBuffer->GetColorAttachment() : TextureHandle{ 0 };
	}

	TextureHandle OpenGL33SceneRecorder::GetEditorDepthAttachment()
	{
		return m_EditorFrameBuffer ? m_EditorFrameBuffer->GetDepthAttachment() : TextureHandle{ 0 };
	}

	void OpenGL33SceneRecorder::SetGameViewportSize(const glm::vec2& gameViewportSize)
	{
		if (!(gameViewportSize.x > 0 && gameViewportSize.x < 8096 && gameViewportSize.y > 0 && gameViewportSize.y < 8096))
			return;
		m_GameViewportSize = gameViewportSize;
		if (m_GameFrameBuffer->GetWidth() != m_GameViewportSize.x || m_GameFrameBuffer->GetHeight() != m_GameViewportSize.y)
			m_RebuildGameImage = true;
	}

	glm::vec2 OpenGL33SceneRecorder::GetGameViewportSize()
	{
		return m_GameViewportSize;
	}

	void OpenGL33SceneRecorder::SetEditorViewportSize(const glm::vec2& editorViewportSize)
	{
		if (!(editorViewportSize.x > 0 && editorViewportSize.x < 8096 && editorViewportSize.y > 0 && editorViewportSize.y < 8096))
			return;
		m_EditorViewportSize = editorViewportSize;
		if (m_EditorFrameBuffer->GetWidth() != m_EditorViewportSize.x || m_EditorFrameBuffer->GetHeight() != m_EditorViewportSize.y)
			m_RebuildEditorImage = true;
	}

	glm::vec2 OpenGL33SceneRecorder::GetEditorViewportSize()
	{
		return m_EditorViewportSize;
	}
}
