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
		glCreateBuffers(1, &m_RendererId);
		glNamedBufferStorage(m_RendererId, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGL46VertexBuffer::~OpenGL46VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGL46VertexBuffer::Bind(unsigned int vertexArray)
	{
		glVertexArrayVertexBuffer(vertexArray, 0, m_RendererId, 0, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)));
	}

	void OpenGL46VertexBuffer::SetData(const Vertex* vertices, size_t vertexCount)
	{
		glNamedBufferSubData(m_RendererId, 0, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices);
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
