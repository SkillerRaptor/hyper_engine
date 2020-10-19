#include "OpenGLIndexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(size_t indexCount)
		: OpenGLIndexBuffer(nullptr, indexCount)
	{
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, size_t indexCount)
		: m_RendererId(0)
	{
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indexCount, indices, (indices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGLIndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* indices, size_t indexCount)
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indexCount, indices);
	}

	unsigned int OpenGLIndexBuffer::GetRendererId() const
	{
		return m_RendererId;
	}
}
