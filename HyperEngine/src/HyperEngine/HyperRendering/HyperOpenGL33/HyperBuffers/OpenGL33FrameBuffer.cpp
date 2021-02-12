#include "HyperRendering/HyperOpenGL33/HyperBuffers/OpenGL33FrameBuffer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HyperCore/Application.hpp"
#include "HyperCore/Core.hpp"

namespace HyperRendering
{
	static constexpr const size_t MAX_FRAMEBUFFER_SIZE = 8192;

	OpenGL33FrameBuffer::OpenGL33FrameBuffer(uint32_t width, uint32_t height)
		: m_Width{ width }, m_Height{ height }
	{
		Build();
	}

	OpenGL33FrameBuffer::~OpenGL33FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGL33FrameBuffer::Build()
	{
		HyperCore::Ref<TextureManager> textureManager = HyperCore::Application::Get()->GetWindow()->GetContext()->GetTextureManager();
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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL33FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Width, m_Height);
	}

	void OpenGL33FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL33FrameBuffer::Resize(uint32_t width, uint32_t height)
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
}
