#pragma once

#include <cstdint>
#include <cstddef>

namespace HyperRendering
{
	class OpenGL33IndexBuffer
	{
	private:
		uint32_t m_RendererID = 0;

	public:
		explicit OpenGL33IndexBuffer(size_t indexCount);
		OpenGL33IndexBuffer(const uint32_t* indices, size_t indexCount);
		~OpenGL33IndexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const uint32_t* indices, size_t indexCount);
	};
}
