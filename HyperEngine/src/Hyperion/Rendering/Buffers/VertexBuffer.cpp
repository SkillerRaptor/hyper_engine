#include "VertexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion 
{
	VertexBuffer::VertexBuffer(VertexLayout vertexLayout, size_t vertexCount)
		: VertexBuffer(vertexLayout, nullptr, vertexCount)
	{
	}

	VertexBuffer::VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount)
		: m_RendererId(0), m_VertexLayout(vertexLayout)
	{
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, (vertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices, (vertices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void VertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void VertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	void VertexBuffer::SetData(const Vertex* vertices, size_t vertexCount)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices);
	}

	unsigned int VertexBuffer::GetRendererId() const
	{
		return m_RendererId;
	}
}
