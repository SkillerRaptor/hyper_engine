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
		: m_RendererID(0)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, sizeof(uint32_t) * indexCount, indices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGL46IndexBuffer::~OpenGL46IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGL46IndexBuffer::Bind(uint32_t vertexArray)
	{
		glVertexArrayElementBuffer(vertexArray, m_RendererID);
	}

	void OpenGL46IndexBuffer::SetData(const uint32_t* indices, size_t indexCount)
	{
		glNamedBufferSubData(m_RendererID, 0, sizeof(uint32_t) * indexCount, indices);
	}
}
