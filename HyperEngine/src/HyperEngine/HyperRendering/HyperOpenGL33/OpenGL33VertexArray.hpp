#pragma once

#include "HyperRendering/Vertex.hpp"

namespace HyperRendering
{
	class OpenGL33VertexArray
	{
	private:
		uint32_t m_RendererID{ 0 };
		VertexLayout m_VertexLayout;

	public:
		OpenGL33VertexArray(VertexLayout vertexLayout);
		~OpenGL33VertexArray();

		void Init();

		void Bind() const;
		void Unbind() const;

		inline VertexLayout GetVertexLayout() const
		{
			return m_VertexLayout;
		}
	};
}
