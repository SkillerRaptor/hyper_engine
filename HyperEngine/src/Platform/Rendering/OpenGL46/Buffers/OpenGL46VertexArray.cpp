#include "OpenGL46VertexArray.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL46VertexArray::OpenGL46VertexArray(VertexLayout vertexLayout)
		: m_RendererId(0), m_VertexLayout(vertexLayout)
	{
		glCreateVertexArrays(1, &m_RendererId);
	}

	OpenGL46VertexArray::~OpenGL46VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererId);
	}

	void OpenGL46VertexArray::Init()
	{
		switch (m_VertexLayout)
		{
		case VertexLayout::Vertex2D:
			glEnableVertexArrayAttrib(m_RendererId, 0);
			glVertexArrayAttribFormat(m_RendererId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, Position));
			glVertexArrayAttribBinding(m_RendererId, 0, 0);

			glEnableVertexArrayAttrib(m_RendererId, 1);
			glVertexArrayAttribFormat(m_RendererId, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, Color));
			glVertexArrayAttribBinding(m_RendererId, 1, 0);

			glEnableVertexArrayAttrib(m_RendererId, 2);
			glVertexArrayAttribFormat(m_RendererId, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, TextureCoords));
			glVertexArrayAttribBinding(m_RendererId, 2, 0);

			glEnableVertexArrayAttrib(m_RendererId, 3);
			glVertexArrayAttribFormat(m_RendererId, 3, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(Vertex2D, TextureId));
			glVertexArrayAttribBinding(m_RendererId, 3, 0);
			break;

		case VertexLayout::Vertex3D:
			glEnableVertexArrayAttrib(m_RendererId, 0);
			glVertexArrayAttribFormat(m_RendererId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Position));
			glVertexArrayAttribBinding(m_RendererId, 0, 0);

			glEnableVertexArrayAttrib(m_RendererId, 1);
			glVertexArrayAttribFormat(m_RendererId, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Color));
			glVertexArrayAttribBinding(m_RendererId, 1, 0);

			glEnableVertexArrayAttrib(m_RendererId, 2);
			glVertexArrayAttribFormat(m_RendererId, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Normals));
			glVertexArrayAttribBinding(m_RendererId, 2, 0);

			glEnableVertexArrayAttrib(m_RendererId, 3);
			glVertexArrayAttribFormat(m_RendererId, 3, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, TextureCoords));
			glVertexArrayAttribBinding(m_RendererId, 3, 0);

			glEnableVertexArrayAttrib(m_RendererId, 4);
			glVertexArrayAttribFormat(m_RendererId, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Tangent));
			glVertexArrayAttribBinding(m_RendererId, 4, 0);

			glEnableVertexArrayAttrib(m_RendererId, 5);
			glVertexArrayAttribFormat(m_RendererId, 5, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Bitangent));
			glVertexArrayAttribBinding(m_RendererId, 5, 0);

			glEnableVertexArrayAttrib(m_RendererId, 6);
			glVertexArrayAttribFormat(m_RendererId, 6, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(Vertex3D, TextureId));
			glVertexArrayAttribBinding(m_RendererId, 6, 0);
			break;
		}
	}

	void OpenGL46VertexArray::Bind()
	{
		glBindVertexArray(m_RendererId);
	}

	void OpenGL46VertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	unsigned int OpenGL46VertexArray::GetRendererId() const
	{
		return m_RendererId;
	}

	VertexLayout OpenGL46VertexArray::GetVertexLayout() const
	{
		return m_VertexLayout;
	}
}
