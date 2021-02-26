#include "HyperRendering/HyperOpenGL33/OpenGL33VertexArray.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperRendering
{
	OpenGL33VertexArray::OpenGL33VertexArray(VertexLayout vertexLayout)
		: m_VertexLayout{ vertexLayout }
	{
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
		glBindVertexArray(0);
	}

	OpenGL33VertexArray::~OpenGL33VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGL33VertexArray::Init()
	{
		switch (m_VertexLayout)
		{
		case VertexLayout::Vertex2D:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, Position));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, Color));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, TextureCoords));
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, TextureId));
			glEnableVertexAttribArray(3);
			break;

		case VertexLayout::Vertex3D:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const void*)offsetof(Vertex3D, Position));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const void*)offsetof(Vertex3D, Color));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const void*)offsetof(Vertex3D, TextureCoords));
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex3D), (const void*)offsetof(Vertex3D, TextureId));
			glEnableVertexAttribArray(3);

			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const void*)offsetof(Vertex3D, Normals));
			glEnableVertexAttribArray(4);

			glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const void*)offsetof(Vertex3D, Tangent));
			glEnableVertexAttribArray(5);

			glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const void*)offsetof(Vertex3D, Bitangent));
			glEnableVertexAttribArray(6);
			break;
		}
	}

	void OpenGL33VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGL33VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
}
