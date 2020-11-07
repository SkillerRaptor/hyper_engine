#include "OpenGL46IndexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL46IndexBuffer::OpenGL46IndexBuffer(size_t indexCount)
		: OpenGL46IndexBuffer(nullptr, indexCount)
	{
	}

	OpenGL46IndexBuffer::OpenGL46IndexBuffer(const uint32_t* indices, size_t indexCount)
		: m_RendererId(0)
	{
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indexCount, indices, (indices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGL46IndexBuffer::~OpenGL46IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGL46IndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGL46IndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGL46IndexBuffer::SetData(const uint32_t* indices, size_t indexCount)
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indexCount, indices);
	}

	unsigned int OpenGL46IndexBuffer::GetRendererId() const
	{
		return m_RendererId;
	}
}
