#pragma once

#include "HyperRendering/Buffers/Vertex.hpp"

namespace Hyperion
{
	class OpenGLVertexArray
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		OpenGLVertexArray(VertexLayout vertexLayout);
		~OpenGLVertexArray();

		void Init();

		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
		VertexLayout GetVertexLayout() const;
	};
}
