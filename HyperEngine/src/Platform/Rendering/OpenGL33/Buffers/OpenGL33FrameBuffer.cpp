#include "OpenGL33FrameBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL33FrameBuffer::OpenGL33FrameBuffer(uint32_t width, uint32_t height)
		: m_RendererId(0), m_Width(width), m_Height(height)
	{
		glGenFramebuffers(1, &m_RendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
	}

	OpenGL33FrameBuffer::~OpenGL33FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererId);
	}

	void OpenGL33FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
		glViewport(0, 0, m_Width, m_Height);
	}

	void OpenGL33FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int OpenGL33FrameBuffer::GetRendererId() const
	{
		return m_RendererId;
	}
}
