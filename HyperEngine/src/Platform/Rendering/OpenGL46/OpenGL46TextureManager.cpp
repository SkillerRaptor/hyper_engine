#include "OpenGL46TextureManager.hpp"

#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "HyperUtilities/Log.hpp"

namespace Hyperion
{
	OpenGL46TextureManager::OpenGL46TextureManager()
	{
		HP_CORE_DEBUG("OpenGL Texture-Manager loaded...");
	}

	OpenGL46TextureManager::~OpenGL46TextureManager()
	{
		HP_CORE_DEBUG("OpenGL Texture-Manager unloaded...");
		for (const auto& [handle, textureData] : m_Textures)
			glDeleteTextures(1, &textureData.TextureId);
		m_Textures.clear();
	}

	uint32_t OpenGL46TextureManager::CreateTexture(const std::string& path, TextureType textureType)
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

		uint32_t textureId = 1;
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

	uint32_t OpenGL46TextureManager::CreateTexture(uint32_t width, uint32_t height, TextureType textureType)
	{
		OpenGLTextureData textureData;
		textureData.Width = width;
		textureData.Height = height;
		textureData.TextureType = textureType;

		GenerateTexture(&textureData, true);

		uint32_t textureId = 1;
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

	void OpenGL46TextureManager::GenerateTexture(TextureData* textureData, bool alpha)
	{
		OpenGLTextureData* data = static_cast<OpenGLTextureData*>(textureData);
		glCreateTextures(GL_TEXTURE_2D, 1, &data->TextureId);
		
		glTextureParameteri(data->TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(data->TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(data->TextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(data->TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureStorage2D(data->TextureId, 1, alpha ? GL_RGBA8 : GL_RGB8, data->Width, data->Height);
		glTextureSubImage2D(data->TextureId, 0, 0, 0, data->Width, data->Height, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data->Data);
	}

	bool OpenGL46TextureManager::BindTexture(uint32_t handle, uint32_t textureSlot)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glBindTextureUnit(textureSlot, m_Textures[handle].TextureId);
		return true;
	}

	bool OpenGL46TextureManager::DeleteTexture(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glDeleteTextures(1, &m_Textures[handle].TextureId);
		m_Textures.erase(handle);
		m_TextureIds.push(handle);
		return true;
	}

	bool OpenGL46TextureManager::DeleteTextureData(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glDeleteTextures(1, &m_Textures[handle].TextureId);
		return true;
	}

	void OpenGL46TextureManager::SetWidth(uint32_t handle, uint32_t width)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].Width = width;
	}

	uint32_t OpenGL46TextureManager::GetWidth(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Width;
	}

	void OpenGL46TextureManager::SetHeight(uint32_t handle, uint32_t height)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].Height = height;
	}

	uint32_t OpenGL46TextureManager::GetHeight(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Height;
	}

	void OpenGL46TextureManager::SetTextureType(uint32_t handle, TextureType textureType)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].TextureType = textureType;
	}

	TextureType OpenGL46TextureManager::GetTextureType(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return TextureType::DEFAULT;
		return m_Textures[handle].TextureType;
	}

	uint8_t OpenGL46TextureManager::GetChannels(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Channels;
	}

	const unsigned char* OpenGL46TextureManager::GetData(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return nullptr;
		return m_Textures[handle].Data;
	}

	const std::string& OpenGL46TextureManager::GetPath(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return std::string();
		return m_Textures[handle].Path;
	}

	TextureData* OpenGL46TextureManager::GetTextureData(uint32_t handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return nullptr;
		return &m_Textures[handle];
	}
}
