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
		: m_RendererID(0), m_VertexLayout(vertexLayout)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices, (vertices == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	}

	OpenGL33VertexBuffer::~OpenGL33VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGL33VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGL33VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGL33VertexBuffer::SetData(const Vertex* vertices, size_t vertexCount)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices);
	}

	VertexLayout OpenGL33VertexBuffer::GetVertexLayout() const
	{
		return m_VertexLayout;
	}
}
