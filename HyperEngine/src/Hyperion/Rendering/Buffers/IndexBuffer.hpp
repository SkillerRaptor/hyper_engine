#pragma once

#include <iostream>

namespace Hyperion 
{
	class IndexBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		IndexBuffer(size_t indexCount);
		IndexBuffer(const uint32_t* indices, size_t indexCount);
		~IndexBuffer();

		void Bind();
		void Unbind();
		void SetData(const uint32_t* indices, size_t indexCount);

		unsigned int GetRendererId() const;
	};
}