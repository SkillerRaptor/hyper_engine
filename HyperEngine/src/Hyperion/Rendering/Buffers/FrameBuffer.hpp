#pragma once

namespace Hyperion
{
	class FrameBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		FrameBuffer();
		~FrameBuffer();

		void Bind();
		void Unbind();

		unsigned int GetRendererId() const;
	};
}