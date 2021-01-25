#pragma once

#include "HyperRendering/TextureManager.hpp"

namespace Hyperion
{
	class OpenGL33FrameBuffer
	{
	private:
		uint32_t m_RendererID = 0;
		TextureHandle m_ColorAttachment = { 0 };
		TextureHandle m_DepthAttachment = { 0 };

		uint32_t m_Width;
		uint32_t m_Height;

	public:
		OpenGL33FrameBuffer(uint32_t width, uint32_t height);
		~OpenGL33FrameBuffer();

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		TextureHandle GetColorAttachment() const;
		TextureHandle GetDepthAttachment() const;

	private:
		void Build();
	};
}
