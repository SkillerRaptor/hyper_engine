#pragma once

#include "HyperRendering/Buffers/Vertex.hpp"

namespace Hyperion
{
	class OpenGL46VertexBuffer
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGL46VertexBuffer(VertexLayout vertexLayout, size_t vertexCount);
		OpenGL46VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount);
		~OpenGL46VertexBuffer();

		void Bind(unsigned int vertexArray);

		void SetData(const Vertex* vertices, size_t vertexCount);

		unsigned int GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
