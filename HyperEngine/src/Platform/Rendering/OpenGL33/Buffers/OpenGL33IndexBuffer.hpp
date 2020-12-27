#pragma once

#include <cstdint>
#include <cstddef>

namespace Hyperion
{
	class OpenGL33IndexBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		explicit OpenGL33IndexBuffer(size_t indexCount);
		OpenGL33IndexBuffer(const uint32_t* indices, size_t indexCount);
		~OpenGL33IndexBuffer();

		void Bind();
		void Unbind();
		void SetData(const uint32_t* indices, size_t indexCount);

		unsigned int GetRendererId() const;
	};
}
