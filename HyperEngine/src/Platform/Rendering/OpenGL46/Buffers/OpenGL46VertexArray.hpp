#pragma once

#include "HyperRendering/Vertex.hpp"

namespace Hyperion
{
	class OpenGL46VertexArray
	{
	private:
		uint32_t m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGL46VertexArray(VertexLayout vertexLayout);
		~OpenGL46VertexArray();

		void Init();

		void Bind();
		void Unbind();

		uint32_t GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
