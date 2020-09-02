#include "Texture.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Hyperion
{
	Texture::Texture()
		: m_TextureID(0), m_TextureType(TextureType::NORMAL), m_Width(0), m_Height(0), m_Channels(0)
	{
	}

	Texture::~Texture()
	{
		if(m_TextureID != 0) glDeleteTextures(1, &m_TextureID);
	}

	bool Texture::LoadTexture(TextureType textureType, const char* filePath, bool alpha)
	{
		unsigned char* textureData = stbi_load(filePath, &m_Width, &m_Height, &m_Channels, 0);
		if (textureData == nullptr) return false;
		GenerateTexture(textureData, alpha);
		stbi_image_free(textureData);

		m_TextureType = textureType;
		return true;
	}

	void Texture::GenerateTexture(unsigned char* textureData, bool alpha)
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, m_Width, m_Height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData);
	}

	void Texture::BindTexture(unsigned int textureIndex) const
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void Texture::SetTextureType(TextureType textureType)
	{
		m_TextureType = textureType;
	}

	TextureType Texture::GetTextureType() const
	{
		return m_TextureType;
	}

	void Texture::SetWidth(unsigned int width)
	{
		m_Width = width;
	}

	int Texture::GetWidth() const
	{
		return m_Width;
	}

	void Texture::SetHeight(unsigned int height)
	{
		m_Height = height;
	}

	int Texture::GetHeight() const
	{
		return m_Height;
	}

	int Texture::GetChannels() const
	{
		return m_Channels;
	}

	unsigned int Texture::GetTextureId() const
	{
		return m_TextureID;
	}
}