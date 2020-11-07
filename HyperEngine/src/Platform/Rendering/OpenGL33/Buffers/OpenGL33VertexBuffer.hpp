#pragma once

#include "HyperRendering/Vertex.hpp"

namespace Hyperion
{
	class OpenGL33VertexBuffer
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGL33VertexBuffer(VertexLayout vertexLayout, size_t vertexCount);
		OpenGL33VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount);
		~OpenGL33VertexBuffer();

		void Bind();
		void Unbind();

		void SetData(const Vertex* vertices, size_t vertexCount);

		unsigned int GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
