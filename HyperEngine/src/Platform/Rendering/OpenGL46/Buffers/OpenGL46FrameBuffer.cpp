#include "OpenGL46FrameBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HyperCore/Application.hpp"
#include "HyperCore/Core.hpp"

namespace Hyperion
{
	static constexpr const size_t MAX_FRAMEBUFFER_SIZE = 8192;

	OpenGL46FrameBuffer::OpenGL46FrameBuffer(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		Build();
	}

	OpenGL46FrameBuffer::~OpenGL46FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGL46FrameBuffer::Build()
	{
		Ref<TextureManager> textureManager = Application::Get()->GetWindow()->GetContext()->GetTextureManager();
		if (m_RendererID != 0)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			textureManager->DeleteTexture(m_ColorAttachment);
			textureManager->DeleteTexture(m_DepthAttachment);
		}

		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		m_ColorAttachment = textureManager->CreateTexture(m_Width, m_Height, TextureType::COLOR);
		m_DepthAttachment = textureManager->CreateTexture(m_Width, m_Height, TextureType::DEPTH_STENCIL);

		HP_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
	}

	void OpenGL46FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGL46FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL46FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			HP_CORE_WARN("Failed to resize framebuffer to the width {} and the height {}.", width, height);
			return;
		}

		m_Width = width;
		m_Height = height;

		Build();
	}

	uint32_t OpenGL46FrameBuffer::GetWidth() const
	{
		return m_Width;
	}

	uint32_t OpenGL46FrameBuffer::GetHeight() const
	{
		return m_Height;
	}

	TextureHandle OpenGL46FrameBuffer::GetColorAttachment() const
	{
		return m_ColorAttachment;
	}

	TextureHandle OpenGL46FrameBuffer::GetDepthAttachment() const
	{
		return m_DepthAttachment;
	}
}
