#include "Texture2D.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Hyperion
{
	Texture2D::Texture2D()
		: m_TextureID(0), m_TextureType(TextureType::NORMAL), m_Width(0), m_Height(0), m_Channels(0)
	{
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	bool Texture2D::LoadTexture(TextureType textureType, const char* filePath, bool alpha)
	{
		unsigned char* textureData = stbi_load(filePath, &m_Width, &m_Height, &m_Channels, 0);
		if (textureData == nullptr) return false;
		GenerateTexture(textureData, alpha);
		stbi_image_free(textureData);

		m_TextureType = textureType;
		return true;
	}

	void Texture2D::GenerateTexture(unsigned char* textureData, bool alpha)
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(textureData != nullptr ? m_TextureID : GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, m_Width, m_Height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData);
	}

	void Texture2D::BindTexture(unsigned int textureIndex) const
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	TextureType Texture2D::GetTextureType() const
	{
		return m_TextureType;
	}

	int Texture2D::GetWidth() const
	{
		return m_Width;
	}

	int Texture2D::GetHeight() const
	{
		return m_Height;
	}

	int Texture2D::GetChannels() const
	{
		return m_Channels;
	}

	unsigned int Texture2D::GetTextureId() const
	{
		return m_TextureID;
	}

	void Texture2D::SetTextureType(TextureType textureType)
	{
		m_TextureType = textureType;
	}

	void Texture2D::SetWidth(unsigned int width)
	{
		m_Width = width;
	}

	void Texture2D::SetHeight(unsigned int height)
	{
		m_Height = height;
	}
}