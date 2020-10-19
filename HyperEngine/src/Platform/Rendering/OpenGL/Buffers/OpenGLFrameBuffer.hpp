#pragma once

namespace Hyperion
{
	class OpenGLFrameBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		OpenGLFrameBuffer();
		~OpenGLFrameBuffer();

		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
	};
}
