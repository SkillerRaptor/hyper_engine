#include "OpenGL33FrameBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL33FrameBuffer::OpenGL33FrameBuffer()
		: m_RendererId(0)
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
