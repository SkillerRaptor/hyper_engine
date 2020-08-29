#include "FrameBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion 
{
	FrameBuffer::FrameBuffer()
		: m_RendererId(0)
	{
		glGenFramebuffers(1, &m_RendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererId);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int FrameBuffer::GetRendererId() const
	{
		return m_RendererId;
	}
}