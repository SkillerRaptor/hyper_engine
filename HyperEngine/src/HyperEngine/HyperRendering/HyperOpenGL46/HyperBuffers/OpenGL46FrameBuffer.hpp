#pragma once

#include "HyperRendering/TextureManager.hpp"

namespace HyperRendering
{
	class OpenGL46FrameBuffer
	{
	private:
		uint32_t m_RendererID = 0;
		TextureHandle m_ColorAttachment{ 0 };
		TextureHandle m_DepthAttachment{ 0 };

		uint32_t m_Width;
		uint32_t m_Height;

	public:
		OpenGL46FrameBuffer(uint32_t width, uint32_t height);
		~OpenGL46FrameBuffer();

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);

		inline uint32_t GetWidth() const
		{
			return m_Width;
		}

		inline uint32_t GetHeight() const
		{
			return m_Height;
		}

		inline TextureHandle GetColorAttachment() const
		{
			return m_ColorAttachment;
		}

		inline TextureHandle GetDepthAttachment() const
		{
			return m_DepthAttachment;
		}

	private:
		void Build();
	};
}
