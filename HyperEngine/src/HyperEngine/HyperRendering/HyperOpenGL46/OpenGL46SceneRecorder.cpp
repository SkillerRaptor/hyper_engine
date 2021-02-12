#include "HyperRendering/HyperOpenGL46/OpenGL46SceneRecorder.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "HyperRendering/HyperOpenGL46/OpenGL46TextureManager.hpp"

namespace HyperRendering
{
	OpenGL46SceneRecorder::OpenGL46SceneRecorder(HyperCore::Ref<TextureManager> textureManager, GLFWwindow* window)
		: SceneRecorder{ textureManager, window }
	{
	}

	void OpenGL46SceneRecorder::InitRecorder()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_Window));

		m_GameFrameBuffer = HyperCore::CreateScope<OpenGL46FrameBuffer>(data.Width, data.Height);
		m_EditorFrameBuffer = HyperCore::CreateScope<OpenGL46FrameBuffer>(data.Width, data.Height);

		m_GameFrameBuffer->Bind();
		glEnable(GL_DEPTH_TEST);
		m_GameFrameBuffer->Unbind();

		m_EditorFrameBuffer->Bind();
		glEnable(GL_DEPTH_TEST);
		m_EditorFrameBuffer->Unbind();
	}

	void OpenGL46SceneRecorder::RebuildRecoder()
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

	void OpenGL46SceneRecorder::StartGameRecording()
	{
		m_GameFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL46SceneRecorder::EndGameRecording()
	{
		m_GameFrameBuffer->Unbind();
	}

	void OpenGL46SceneRecorder::RenderGameImage()
	{
		ImGui::Image(m_TextureManager->GetImageTextureId(m_GameFrameBuffer->GetColorAttachment()), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}

	void OpenGL46SceneRecorder::StartEditorRecording()
	{
		m_EditorFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL46SceneRecorder::EndEditorRecording()
	{
		m_EditorFrameBuffer->Unbind();
	}

	void OpenGL46SceneRecorder::RenderEditorImage()
	{
		ImGui::Image(m_TextureManager->GetImageTextureId(m_EditorFrameBuffer->GetColorAttachment()), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}
}
