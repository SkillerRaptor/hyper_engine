#include "OpenGL33VertexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL33VertexBuffer::OpenGL33VertexBuffer(VertexLayout vertexLayout, size_t vertexCount)
		: OpenGL33VertexBuffer(vertexLayout, nullptr, vertexCount)
	{
	}

	OpenGL33VertexBuffer::OpenGL33VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount)
		: m_RendererId(0), m_VertexLayout(vertexLayout)
	{
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices, (vertices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGL33VertexBuffer::~OpenGL33VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGL33VertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGL33VertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGL33VertexBuffer::SetData(const Vertex* vertices, size_t vertexCount)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices);
	}

	uint32_t OpenGL33VertexBuffer::GetRendererId() const
	{
		return m_RendererId;
	}

	VertexLayout OpenGL33VertexBuffer::GetVertexLayout() const
	{
		return m_VertexLayout;
	}
}
