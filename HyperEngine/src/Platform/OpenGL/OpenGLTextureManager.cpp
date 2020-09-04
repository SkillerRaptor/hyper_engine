#include "OpenGLTextureManager.hpp"

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
		for (std::unordered_map<uint32_t, OpenGLTexture>::iterator it = m_Textures.begin(); it != m_Textures.end(); it++)
			it->second.DeleteTexture();
		m_Textures.clear();
	}

	uint32_t OpenGLTextureManager::CreateTexture(const std::string& path, TextureType textureType)
	{
		OpenGLTexture texture = OpenGLTexture(path, textureType);
		if (!texture.LoadTexture())
		{
			HP_CORE_DEBUG("Texture not loaded...");
			return -1;
		}

		uint32_t textureId = 0;
		if (!m_TextureIds.empty())
		{
			textureId = m_TextureIds.front();
			m_TextureIds.pop();
		}
		else
			textureId = static_cast<uint32_t>(m_Textures.size());
		m_Textures.emplace(textureId, std::move(texture));
		HP_CORE_DEBUG("Texture % loaded...");
		return textureId;
	}

	uint32_t OpenGLTextureManager::CreateTexture(uint32_t width, uint32_t height, TextureType textureType)
	{
		OpenGLTexture texture = OpenGLTexture(width, height, textureType);
		if (!texture.LoadTexture())
		{
			HP_CORE_DEBUG("Texture not loaded...");
			return -1;
		}

		uint32_t textureId = 0;
		if (!m_TextureIds.empty())
		{
			textureId = m_TextureIds.front();
			m_TextureIds.pop();
		}
		else
			textureId = static_cast<uint32_t>(m_Textures.size());
		m_Textures.emplace(textureId, std::move(texture));
		HP_CORE_DEBUG("Texture % loaded...");
		return textureId;
	}

	bool OpenGLTextureManager::BindTexture(uint32_t textureIndex, uint32_t textureSlot)
	{
		if (m_Textures.find(textureIndex) == m_Textures.end())
			return false;
		m_Textures[textureIndex].BindTexture(textureSlot);
		return true;
	}

	bool OpenGLTextureManager::DeleteTexture(uint32_t textureIndex)
	{
		if (m_Textures.find(textureIndex) == m_Textures.end())
			return false;
		m_Textures[textureIndex].DeleteTexture();
		m_Textures.erase(textureIndex);
		m_TextureIds.push(textureIndex);
		return true;
	}

	Texture* OpenGLTextureManager::GetTexture(uint32_t textureIndex)
	{
		if (m_Textures.find(textureIndex) == m_Textures.end())
			return nullptr;
		return static_cast<Texture*>(&m_Textures[textureIndex]);
	}
}