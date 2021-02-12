#pragma once

#include "HyperRendering/Vertex.hpp"

namespace HyperRendering
{
	class OpenGL33VertexBuffer
	{
	private:
		uint32_t m_RendererID{ 0 };
		VertexLayout m_VertexLayout;

	public:
		OpenGL33VertexBuffer(VertexLayout vertexLayout, size_t vertexCount);
		OpenGL33VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount);
		~OpenGL33VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const Vertex* vertices, size_t vertexCount);

		inline VertexLayout GetVertexLayout() const
		{
			return m_VertexLayout;
		}
	};
}
