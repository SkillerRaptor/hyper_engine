#pragma once

#include <cstdint>
#include <cstddef>

namespace Hyperion
{
	class OpenGL46IndexBuffer
	{
	private:
		uint32_t m_RendererId;

	public:
		explicit OpenGL46IndexBuffer(size_t indexCount);
		OpenGL46IndexBuffer(const uint32_t* indices, size_t indexCount);
		~OpenGL46IndexBuffer();

		void Bind(uint32_t vertexArray);

		void SetData(const uint32_t* indices, size_t indexCount);

		uint32_t GetRendererId() const;
	};
}
