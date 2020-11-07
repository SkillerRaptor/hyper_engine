#pragma once

#include "HyperRendering/Buffers/Vertex.hpp"

namespace Hyperion
{
	class OpenGL33VertexArray
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGL33VertexArray(VertexLayout vertexLayout);
		~OpenGL33VertexArray();

		void Init();

		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
