#include "OpenGLVertexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(VertexLayout vertexLayout, size_t vertexCount)
		: OpenGLVertexBuffer(vertexLayout, nullptr, vertexCount)
	{
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount)
		: m_RendererId(0), m_VertexLayout(vertexLayout)
	{
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_VertexLayout == VertexLayout::Vertex2D ? VertexLayout::Vertex2D : VertexLayout::Vertex3D) * vertexCount, vertices, (vertices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGLVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGLVertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const Vertex* vertices, size_t vertexCount)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_VertexLayout == VertexLayout::Vertex2D ? VertexLayout::Vertex2D : VertexLayout::Vertex3D) * vertexCount, vertices);
	}

	unsigned int OpenGLVertexBuffer::GetRendererId() const
	{
		return m_RendererId;
	}

	VertexLayout OpenGLVertexBuffer::GetVertexLayout() const
	{
		return m_VertexLayout;
	}
}
