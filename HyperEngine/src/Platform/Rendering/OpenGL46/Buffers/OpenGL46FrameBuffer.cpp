#include "OpenGL46FrameBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL46FrameBuffer::OpenGL46FrameBuffer()
		: m_RendererId(0)
	{
		glGenFramebuffers(1, &m_RendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
	}

	OpenGL46FrameBuffer::~OpenGL46FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererId);
	}

	void OpenGL46FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
	}

	void OpenGL46FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int OpenGL46FrameBuffer::GetRendererId() const
	{
		return m_RendererId;
	}
}
