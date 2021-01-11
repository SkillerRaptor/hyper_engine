#pragma once

#include <cstdint>

namespace Hyperion
{
	class OpenGL33FrameBuffer
	{
	private:
		uint32_t m_RendererId;

		uint32_t m_Width;
		uint32_t m_Height;

	public:
		OpenGL33FrameBuffer(uint32_t width, uint32_t height);
		~OpenGL33FrameBuffer();

		void Bind();
		void Unbind();

		uint32_t GetRendererId() const;
	};
}
