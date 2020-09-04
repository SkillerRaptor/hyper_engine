#include "OpenGLTexture.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Hyperion
{
	OpenGLTexture::OpenGLTexture(const std::string& path, TextureType textureType)
		: Texture(path, textureType)
	{
	}

	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, TextureType textureType)
		: Texture(width, height, textureType)
	{
	}

	bool OpenGLTexture::LoadTexture()
	{
		int width, height, channels;
		m_Data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
		if (m_Data == nullptr) return false;

		m_Width = width;
		m_Height = height;
		m_Channels = channels;

		GenerateTexture(channels >= 4 ? true : false);
		stbi_image_free(m_Data);
		return true;
	}

	void OpenGLTexture::GenerateTexture(bool alpha)
	{
		glGenTextures(1, &m_TextureId);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);

		glTexParameteri(m_Data != nullptr ? m_TextureId : GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_Data != nullptr ? m_TextureId : GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_Data != nullptr ? m_TextureId : GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(m_Data != nullptr ? m_TextureId : GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, m_Width, m_Height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, m_Data);
	}

	void OpenGLTexture::DeleteTexture()
	{
		if (m_TextureId != 0) glDeleteTextures(1, &m_TextureId);
	}

	void OpenGLTexture::BindTexture(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
	}
}