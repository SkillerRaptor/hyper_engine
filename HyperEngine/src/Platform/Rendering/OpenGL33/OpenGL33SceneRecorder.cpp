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

	void OpenGL33SceneRecorder::InitRecording()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_Window));
		m_FrameBuffer = CreateScope<OpenGL33FrameBuffer>(data.Width, data.Height);

		m_FrameBuffer->Bind();
		glEnable(GL_DEPTH_TEST);
		m_FrameBuffer->Unbind();
	}

	void OpenGL33SceneRecorder::StartRecording()
	{
		WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(m_Window));

		if (m_FrameBuffer->GetWidth() != data.Width || m_FrameBuffer->GetHeight() != data.Height)
			m_FrameBuffer->Resize(data.Width, data.Height);

		m_FrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL33SceneRecorder::EndRecording()
	{
		m_FrameBuffer->Unbind();
	}

	void OpenGL33SceneRecorder::RenderImage()
	{
		uint32_t colorTextureId = static_cast<OpenGLTextureData*>(m_TextureManager->GetTextureData(m_FrameBuffer->GetColorAttachment()))->TextureId;
		ImGui::Image((ImTextureID)(intptr_t)colorTextureId, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}	
}
