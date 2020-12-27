#pragma once

#include <cstdint>
#include <cstddef>

namespace Hyperion
{
	class OpenGL46IndexBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		explicit OpenGL46IndexBuffer(size_t indexCount);
		OpenGL46IndexBuffer(const uint32_t* indices, size_t indexCount);
		~OpenGL46IndexBuffer();

		void Bind(unsigned int vertexArray);

		void SetData(const uint32_t* indices, size_t indexCount);

		unsigned int GetRendererId() const;
	};
}
