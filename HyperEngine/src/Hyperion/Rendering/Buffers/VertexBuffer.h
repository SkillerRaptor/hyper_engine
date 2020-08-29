#pragma once

#include "Vertex.h"

namespace Hyperion 
{
	class VertexBuffer
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		VertexBuffer(VertexLayout vertexLayout, size_t vertexCount);
		VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount);
		~VertexBuffer();

		void Bind();
		void Unbind();
		void SetData(const Vertex* vertices, size_t vertexCount);

		unsigned int GetRendererId() const;
	};
}
