#include "HyperRendering/HyperOpenGL33/HyperBuffers/OpenGL33IndexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperRendering
{
	OpenGL33IndexBuffer::OpenGL33IndexBuffer(size_t indexCount)
		: OpenGL33IndexBuffer{ nullptr, indexCount }
	{
	}

	OpenGL33IndexBuffer::OpenGL33IndexBuffer(const uint32_t* indices, size_t indexCount)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indexCount, indices, (indices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGL33IndexBuffer::~OpenGL33IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGL33IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGL33IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGL33IndexBuffer::SetData(const uint32_t* indices, size_t indexCount)
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indexCount, indices);
	}
}
