#include "HyperRendering/HyperOpenGL46/HyperBuffers/OpenGL46VertexBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperRendering
{
	OpenGL46VertexBuffer::OpenGL46VertexBuffer(VertexLayout vertexLayout, size_t vertexCount)
		: OpenGL46VertexBuffer{ vertexLayout, nullptr, vertexCount }
	{
	}

	OpenGL46VertexBuffer::OpenGL46VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount)
		: m_VertexLayout{ vertexLayout }
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGL46VertexBuffer::~OpenGL46VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGL46VertexBuffer::Bind(uint32_t vertexArray)
	{
		glVertexArrayVertexBuffer(vertexArray, 0, m_RendererID, 0, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)));
	}

	void OpenGL46VertexBuffer::SetData(const Vertex* vertices, size_t vertexCount)
	{
		glNamedBufferSubData(m_RendererID, 0, (m_VertexLayout == VertexLayout::Vertex2D ? sizeof(Vertex2D) : sizeof(Vertex3D)) * vertexCount, vertices);
	}
}
