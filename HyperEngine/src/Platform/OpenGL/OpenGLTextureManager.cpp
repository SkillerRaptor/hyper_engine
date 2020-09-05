#include "OpenGLTextureManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Utilities/Log.hpp"

namespace Hyperion
{
	OpenGLTextureManager::OpenGLTextureManager()
	{
		HP_CORE_DEBUG("OpenGL Texture-Manager loaded...");
	}

	OpenGLTextureManager::~OpenGLTextureManager()
	{
		HP_CORE_DEBUG("OpenGL Texture-Manager unloaded...");
		for (const auto& [handle, textureData] : m_Textures)
			glDeleteTextures(1, &textureData.TextureId);
		m_Textures.clear();
	}

	uint32_t OpenGLTextureManager::CreateTexture(const std::string& path, TextureType textureType)
	{
		OpenGLTextureData textureData;
		textureData.Path = path;
		textureData.TextureType = textureType;

		int width, height, channels;
		textureData.Data = stbi_load(textureData.Path.c_str(), &width, &height, &channels, 0);
		if (textureData.Data == nullptr)
		{
			HP_CORE_DEBUG("Texture not loaded...");
			return -1;
		}

		textureData.Width = width;
		textureData.Height = height;
		textureData.Channels = channels;
		
		glGenTextures(1, &textureData.TextureId);
		glBindTexture(GL_TEXTURE_2D, textureData.TextureId);

		glTexParameteri(textureData.TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureData.TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(textureData.TextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(textureData.TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, textureData.Channels >= 4 ? GL_RGBA : GL_RGB, textureData.Width, textureData.Height, 0, textureData.Channels >= 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData.Data);

		stbi_image_free(textureData.Data);

		uint32_t textureId = 0;
		if (!m_TextureIds.empty())
		{
			textureId = m_TextureIds.front();
			m_TextureIds.pop();
		}
		else
			textureId = static_cast<uint32_t>(m_Textures.size());
		m_Textures.emplace(textureId, std::move(textureData));
		HP_CORE_DEBUG("Texture % loaded...");
		return textureId;
	}

	uint32_t OpenGLTextureManager::CreateTexture(uint32_t width, uint32_t height, TextureType textureType)
	{
		OpenGLTextureData textureData;
		textureData.Width = width;
		textureData.Height = height;
		textureData.TextureType = textureType;

		glGenTextures(1, &textureData.TextureId);
		glBindTexture(GL_TEXTURE_2D, textureData.TextureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData.Width, textureData.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		uint32_t textureId = 0;
		if (!m_TextureIds.empty())
		{
			textureId = m_TextureIds.front();
			m_TextureIds.pop();
		}
		else
			textureId = static_cast<uint32_t>(m_Textures.size());
		m_Textures.emplace(textureId, std::move(textureData));
		HP_CORE_DEBUG("Texture % created...");
		return textureId;
	}

	bool OpenGLTextureManager::BindTexture(uint32_t handle, uint32_t textureSlot)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, m_Textures[handle].TextureId);
		return true;
	}

	bool OpenGLTextureManager::DeleteTexture(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glDeleteTextures(1, &m_Textures[handle].TextureId);
		m_Textures.erase(handle);
		m_TextureIds.push(handle);
		return true;
	}

	uint32_t OpenGLTextureManager::GetWidth(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Width;
	}

	uint32_t OpenGLTextureManager::GetHeight(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Height;
	}

	uint8_t OpenGLTextureManager::GetChannels(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Channels;
	}

	unsigned char* OpenGLTextureManager::GetData(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return nullptr;
		return m_Textures[handle].Data;
	}

	std::string OpenGLTextureManager::GetPath(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return "";
		return m_Textures[handle].Path;
	}

	TextureType OpenGLTextureManager::GetTextureType(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return TextureType::DEFAULT;
		return m_Textures[handle].TextureType;
	}
}