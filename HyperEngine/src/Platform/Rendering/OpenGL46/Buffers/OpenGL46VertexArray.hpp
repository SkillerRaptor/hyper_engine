#pragma once

#include "HyperRendering/Vertex.hpp"

namespace Hyperion
{
	class OpenGL46VertexArray
	{
	private:
		uint32_t m_RendererID = 0;
		VertexLayout m_VertexLayout;

		friend class OpenGL46Renderer2D;

	public:
		OpenGL46VertexArray(VertexLayout vertexLayout);
		~OpenGL46VertexArray();

		void Init();

		void Bind();
		void Unbind();

		VertexLayout GetVertexLayout() const;
	};
}
