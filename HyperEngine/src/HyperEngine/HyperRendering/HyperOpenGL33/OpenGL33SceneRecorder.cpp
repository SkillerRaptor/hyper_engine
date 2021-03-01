#include "HyperRendering/HyperOpenGL33/OpenGL33SceneRecorder.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "HyperRendering/HyperOpenGL33/OpenGL33TextureManager.hpp"

namespace HyperRendering
{
	OpenGL33SceneRecorder::OpenGL33SceneRecorder(HyperCore::Ref<TextureManager> textureManager, GLFWwindow* window)
		: SceneRecorder{ textureManager, window }
	{
	}

	void OpenGL33SceneRecorder::InitRecorder()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_Window));

		m_GameFrameBuffer = HyperCore::CreateScope<OpenGL33FrameBuffer>(data.Width, data.Height);
		m_EditorFrameBuffer = HyperCore::CreateScope<OpenGL33FrameBuffer>(data.Width, data.Height);

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
		ImGui::Image(m_TextureManager->GetImageId(m_GameFrameBuffer->GetColorAttachment()), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
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
		ImGui::Image(m_TextureManager->GetImageId(m_EditorFrameBuffer->GetColorAttachment()), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}
}
