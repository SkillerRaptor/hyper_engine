#pragma once

#include "HyperRendering/Vertex.hpp"

namespace Hyperion
{
	class OpenGL33VertexArray
	{
	private:
		uint32_t m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGL33VertexArray(VertexLayout vertexLayout);
		~OpenGL33VertexArray();

		void Init();

		void Bind();
		void Unbind();

		uint32_t GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
