#pragma once

#include "HyperRendering/Vertex.hpp"

namespace HyperRendering
{
	class OpenGL46VertexBuffer
	{
	private:
		uint32_t m_RendererID{ 0 };
		VertexLayout m_VertexLayout;

	public:
		OpenGL46VertexBuffer(VertexLayout vertexLayout, size_t vertexCount);
		OpenGL46VertexBuffer(VertexLayout vertexLayout, const Vertex* vertices, size_t vertexCount);
		~OpenGL46VertexBuffer();

		void Bind(uint32_t vertexArray);

		void SetData(const Vertex* vertices, size_t vertexCount);

		inline VertexLayout GetVertexLayout() const
		{
			return m_VertexLayout;
		}
	};
}
