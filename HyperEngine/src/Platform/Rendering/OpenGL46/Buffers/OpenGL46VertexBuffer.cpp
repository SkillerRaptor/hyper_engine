#include "OpenGL46VertexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL46VertexBuffer::OpenGL46VertexBuffer(VertexLayout vertexLayout, size_t vertexCount)
		: OpenGL46VertexBuffer(vertexLayout, nullptr, vertexCount)
	{
	}

	OpenGL46VertexBuffer::OpenGL46VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount)
		: m_RendererId(0), m_VertexLayout(vertexLayout)
	{
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_VertexLayout == VertexLayout::Vertex2D ? VertexLayout::Vertex2D : VertexLayout::Vertex3D) * vertexCount, vertices, (vertices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGL46VertexBuffer::~OpenGL46VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGL46VertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGL46VertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGL46VertexBuffer::SetData(const Vertex* vertices, size_t vertexCount)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_VertexLayout == VertexLayout::Vertex2D ? VertexLayout::Vertex2D : VertexLayout::Vertex3D) * vertexCount, vertices);
	}

	unsigned int OpenGL46VertexBuffer::GetRendererId() const
	{
		return m_RendererId;
	}

	VertexLayout OpenGL46VertexBuffer::GetVertexLayout() const
	{
		return m_VertexLayout;
	}
}
