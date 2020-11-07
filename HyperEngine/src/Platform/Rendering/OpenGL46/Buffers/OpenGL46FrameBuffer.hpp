#pragma once

namespace Hyperion
{
	class OpenGL46FrameBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		OpenGL46FrameBuffer();
		~OpenGL46FrameBuffer();

		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
	};
}
