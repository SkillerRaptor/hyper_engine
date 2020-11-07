#pragma once

#include "HyperRendering/Buffers/Vertex.hpp"

namespace Hyperion
{
	class OpenGL46VertexArray
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGL46VertexArray(VertexLayout vertexLayout);
		~OpenGL46VertexArray();

		void Init();

		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
