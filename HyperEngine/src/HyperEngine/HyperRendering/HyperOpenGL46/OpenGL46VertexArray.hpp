#pragma once

#include "HyperRendering/Vertex.hpp"

namespace HyperRendering
{
	class OpenGL46VertexArray
	{
	private:
		uint32_t m_RendererID{ 0 };
		VertexLayout m_VertexLayout;

		friend class OpenGL46Renderer2D;

	public:
		OpenGL46VertexArray(VertexLayout vertexLayout);
		~OpenGL46VertexArray();

		void Init();

		void Bind();
		void Unbind();

		inline VertexLayout GetVertexLayout() const
		{
			return m_VertexLayout;
		}
	};
}
