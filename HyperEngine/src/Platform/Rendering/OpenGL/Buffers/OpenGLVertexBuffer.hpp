#pragma once

#include "HyperRendering/Buffers/Vertex.hpp"

namespace Hyperion
{
	class OpenGLVertexBuffer
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGLVertexBuffer(VertexLayout vertexLayout, size_t vertexCount);
		OpenGLVertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount);
		~OpenGLVertexBuffer();

		void Bind();
		void Unbind();

		void SetData(const Vertex* vertices, size_t vertexCount);

		unsigned int GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
