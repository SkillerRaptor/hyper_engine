#pragma once

namespace Hyperion
{
	class OpenGL33FrameBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		OpenGL33FrameBuffer();
		~OpenGL33FrameBuffer();

		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
	};
}
