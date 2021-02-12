#include "HyperRendering/HyperOpenGL46/HyperBuffers/OpenGL46VertexArray.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperRendering
{
	OpenGL46VertexArray::OpenGL46VertexArray(VertexLayout vertexLayout)
		: m_VertexLayout{ vertexLayout }
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGL46VertexArray::~OpenGL46VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGL46VertexArray::Init()
	{
		switch (m_VertexLayout)
		{
		case VertexLayout::Vertex2D:
			glEnableVertexArrayAttrib(m_RendererID, 0);
			glVertexArrayAttribFormat(m_RendererID, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, Position));
			glVertexArrayAttribBinding(m_RendererID, 0, 0);

			glEnableVertexArrayAttrib(m_RendererID, 1);
			glVertexArrayAttribFormat(m_RendererID, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, Color));
			glVertexArrayAttribBinding(m_RendererID, 1, 0);

			glEnableVertexArrayAttrib(m_RendererID, 2);
			glVertexArrayAttribFormat(m_RendererID, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, TextureCoords));
			glVertexArrayAttribBinding(m_RendererID, 2, 0);

			glEnableVertexArrayAttrib(m_RendererID, 3);
			glVertexArrayAttribFormat(m_RendererID, 3, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, TextureId));
			glVertexArrayAttribBinding(m_RendererID, 3, 0);
			break;

		case VertexLayout::Vertex3D:
			glEnableVertexArrayAttrib(m_RendererID, 0);
			glVertexArrayAttribFormat(m_RendererID, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Position));
			glVertexArrayAttribBinding(m_RendererID, 0, 0);

			glEnableVertexArrayAttrib(m_RendererID, 1);
			glVertexArrayAttribFormat(m_RendererID, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Color));
			glVertexArrayAttribBinding(m_RendererID, 1, 0);

			glEnableVertexArrayAttrib(m_RendererID, 2);
			glVertexArrayAttribFormat(m_RendererID, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, TextureCoords));
			glVertexArrayAttribBinding(m_RendererID, 2, 0);

			glEnableVertexArrayAttrib(m_RendererID, 3);
			glVertexArrayAttribFormat(m_RendererID, 3, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(Vertex3D, TextureId));
			glVertexArrayAttribBinding(m_RendererID, 3, 0);

			glEnableVertexArrayAttrib(m_RendererID, 4);
			glVertexArrayAttribFormat(m_RendererID, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Normals));
			glVertexArrayAttribBinding(m_RendererID, 4, 0);

			glEnableVertexArrayAttrib(m_RendererID, 5);
			glVertexArrayAttribFormat(m_RendererID, 5, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Tangent));
			glVertexArrayAttribBinding(m_RendererID, 5, 0);

			glEnableVertexArrayAttrib(m_RendererID, 6);
			glVertexArrayAttribFormat(m_RendererID, 6, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, Bitangent));
			glVertexArrayAttribBinding(m_RendererID, 6, 0);
			break;
		}
	}

	void OpenGL46VertexArray::Bind()
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGL46VertexArray::Unbind()
	{
		glBindVertexArray(0);
	}
}
