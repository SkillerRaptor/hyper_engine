#pragma once

#include "Vertex.hpp"

namespace Hyperion 
{
	class VertexArray
	{
	private:
		unsigned int m_RendererId;
		VertexLayout m_VertexLayout;

	public:
		VertexArray(VertexLayout vertexLayout);
		~VertexArray();

		void Init();
		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
	};
}