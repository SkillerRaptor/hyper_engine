#pragma once

#include <cstdint>

namespace Hyperion
{
	class OpenGL46FrameBuffer
	{
	private:
		uint32_t m_RendererId;

	public:
		OpenGL46FrameBuffer();
		~OpenGL46FrameBuffer();

		void Bind();
		void Unbind();

		uint32_t GetRendererId() const;
	};
}
