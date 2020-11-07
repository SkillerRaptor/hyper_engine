#include "OpenGL33TextureManager.hpp"

#include <stb_image.h>

#include "HyperUtilities/Log.hpp"

namespace Hyperion
{
	OpenGL33TextureManager::OpenGL33TextureManager()
	{
		HP_CORE_DEBUG("OpenGL Texture-Manager loaded...");
	}

	OpenGL33TextureManager::~OpenGL33TextureManager()
	{
		HP_CORE_DEBUG("OpenGL Texture-Manager unloaded...");
		for (const auto& [handle, textureData] : m_Textures)
			glDeleteTextures(1, &textureData.TextureId);
		m_Textures.clear();
	}

	uint32_t OpenGL33TextureManager::CreateTexture(const std::string& path, TextureType textureType)
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

		GenerateTexture(&textureData, textureData.Channels >= 4);
		
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
		HP_CORE_DEBUG("Texture % loaded...", textureId);
		return textureId;
	}

	uint32_t OpenGL33TextureManager::CreateTexture(uint32_t width, uint32_t height, TextureType textureType)
	{
		OpenGLTextureData textureData;
		textureData.Width = width;
		textureData.Height = height;
		textureData.TextureType = textureType;

		GenerateTexture(&textureData, true);

		uint32_t textureId = 0;
		if (!m_TextureIds.empty())
		{
			textureId = m_TextureIds.front();
			m_TextureIds.pop();
		}
		else
			textureId = static_cast<uint32_t>(m_Textures.size());
		m_Textures.emplace(textureId, std::move(textureData));
		HP_CORE_DEBUG("Texture % created...", textureId);
		return textureId;
	}

	void OpenGL33TextureManager::GenerateTexture(TextureData* textureData, bool alpha)
	{
		OpenGLTextureData* data = static_cast<OpenGLTextureData*>(textureData);
		glGenTextures(1, &data->TextureId);
		glBindTexture(GL_TEXTURE_2D, data->TextureId);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, data->Width, data->Height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data->Data);
	}

	bool OpenGL33TextureManager::BindTexture(uint32_t handle, uint32_t textureSlot)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, m_Textures[handle].TextureId);
		return true;
	}

	bool OpenGL33TextureManager::DeleteTexture(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glDeleteTextures(1, &m_Textures[handle].TextureId);
		m_Textures.erase(handle);
		m_TextureIds.push(handle);
		return true;
	}

	bool OpenGL33TextureManager::DeleteTextureData(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glDeleteTextures(1, &m_Textures[handle].TextureId);
		return true;
	}

	void OpenGL33TextureManager::SetWidth(uint32_t handle, uint32_t width)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].Width = width;
	}

	uint32_t OpenGL33TextureManager::GetWidth(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Width;
	}

	void OpenGL33TextureManager::SetHeight(uint32_t handle, uint32_t height)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].Height = height;
	}

	uint32_t OpenGL33TextureManager::GetHeight(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Height;
	}

	uint8_t OpenGL33TextureManager::GetChannels(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Channels;
	}

	const unsigned char* OpenGL33TextureManager::GetData(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return nullptr;
		return m_Textures[handle].Data;
	}

	const std::string OpenGL33TextureManager::GetPath(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return std::string();
		return m_Textures[handle].Path;
	}

	void OpenGL33TextureManager::SetTextureType(uint32_t handle, TextureType textureType)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].TextureType = textureType;
	}

	TextureType OpenGL33TextureManager::GetTextureType(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return TextureType::DEFAULT;
		return m_Textures[handle].TextureType;
	}

	TextureData* OpenGL33TextureManager::GetTextureData(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return nullptr;
		return &m_Textures[handle];
	}
}
