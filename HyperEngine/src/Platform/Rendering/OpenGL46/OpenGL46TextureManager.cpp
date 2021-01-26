#include "OpenGL46TextureManager.hpp"

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

	TextureHandle OpenGL46TextureManager::CreateTexture(const std::string& path, TextureType textureType)
	{
		OpenGLTextureData textureData;
		textureData.Path = path;
		textureData.Type = textureType;

		int width, height, channels;
		unsigned char* pixels = LoadImage(textureData.Path, width, height, channels);
		if (pixels == nullptr)
		{
			HP_CORE_DEBUG("Texture not loaded...");
			return { static_cast<uint32_t>(-1) };
		}

		textureData.Width = width;
		textureData.Height = height;
		textureData.Channels = channels;

		GenerateTexture(&textureData, pixels);

		FreeImage(pixels);

		TextureHandle textureId = { 1 };
		if (!m_TextureIds.empty())
		{
			textureId = m_TextureIds.front();
			m_TextureIds.pop();
		}
		else
			textureId = static_cast<TextureHandle>(TextureHandle{ static_cast<uint32_t>(m_Textures.size()) });
		m_Textures.emplace(textureId, std::move(textureData));
		HP_CORE_DEBUG("Texture {} loaded...", textureId.Handle);
		return textureId;
	}

	TextureHandle OpenGL46TextureManager::CreateTexture(uint32_t width, uint32_t height, TextureType textureType)
	{
		OpenGLTextureData textureData;
		textureData.Width = width;
		textureData.Height = height;
		textureData.Type  = textureType;
		textureData.Channels = 4;

		GenerateTexture(&textureData, nullptr);

		TextureHandle textureId = { 1 };
		if (!m_TextureIds.empty())
		{
			textureId = m_TextureIds.front();
			m_TextureIds.pop();
		}
		else
			textureId = static_cast<TextureHandle>(TextureHandle{ static_cast<uint32_t>(m_Textures.size()) });
		m_Textures.emplace(textureId, std::move(textureData));
		HP_CORE_DEBUG("Texture {} created...", textureId.Handle);
		return textureId;
	}

	void OpenGL46TextureManager::GenerateTexture(TextureData* textureData, unsigned char* pixels)
	{
		OpenGLTextureData* data = static_cast<OpenGLTextureData*>(textureData);
		glCreateTextures(GL_TEXTURE_2D, 1, &data->TextureId);
		
		glTextureParameteri(data->TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(data->TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(data->TextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(data->TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		switch (textureData->Type)
		{
		case TextureType::COMPUTE:
			HP_CORE_WARN("Compute texture type is (yet) not supported in OpenGL 4.6!");
			break;
		case TextureType::DEFAULT:
			glTextureStorage2D(data->TextureId, 1, textureData->Channels >= 4 ? GL_RGBA8 : GL_RGB8, data->Width, data->Height);
			glTextureSubImage2D(data->TextureId, 0, 0, 0, data->Width, data->Height, textureData->Channels >= 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
			break;
		case TextureType::DIFFUSE:
			HP_CORE_WARN("Diffuse texture type is (yet) not supported in OpenGL 4.6!");
			break;
		case TextureType::COLOR:
			glTextureStorage2D(data->TextureId, 1, GL_RGB8, data->Width, data->Height);
			glTextureSubImage2D(data->TextureId, 0, 0, 0, data->Width, data->Height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			//glNamedFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data->TextureId);
			HP_CORE_WARN("Color texture type is (yet) not fully supported in OpenGL 4.6!");
			break;
		case TextureType::DEPTH:
			HP_CORE_WARN("Depth texture type is (yet) not supported in OpenGL 4.6!");
			break;
		case TextureType::STENCIL:
			HP_CORE_WARN("Stencil texture type is (yet) not supported in OpenGL 4.6!");
			break;
		case TextureType::DEPTH_STENCIL:
			glTextureStorage2D(data->TextureId, 1, GL_DEPTH24_STENCIL8, data->Width, data->Height);
			glTextureSubImage2D(data->TextureId, 0, 0, 0, data->Width, data->Height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
			//glNamedFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, data->TextureId);
			HP_CORE_WARN("Depth Stencil texture type is (yet) not fully supported in OpenGL 4.6!");
			break;
		case TextureType::HEIGHT:
			HP_CORE_WARN("Height texture type is (yet) not supported in OpenGL 4.6!");
			break;
		case TextureType::NORMAL:
			HP_CORE_WARN("Normal texture type is (yet) not supported in OpenGL 4.6!");
			break;
		case TextureType::SPECULAR:
			HP_CORE_WARN("Specular texture type is (yet) not supported in OpenGL 4.6!");
			break;
		default:
			break;
		}
	}

	bool OpenGL46TextureManager::BindTexture(TextureHandle handle, uint32_t textureSlot)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glBindTextureUnit(textureSlot, m_Textures[handle].TextureId);
		return true;
	}

	bool OpenGL46TextureManager::DeleteTexture(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glDeleteTextures(1, &m_Textures[handle].TextureId);
		m_Textures.erase(handle);
		m_TextureIds.push(handle);
		return true;
	}

	bool OpenGL46TextureManager::DeleteTextureData(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return false;
		glDeleteTextures(1, &m_Textures[handle].TextureId);
		HP_CORE_DEBUG("Texture {} deleted...", handle.Handle);
		return true;
	}

	void OpenGL46TextureManager::SetWidth(TextureHandle handle, uint32_t width)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].Width = width;
	}

	uint32_t OpenGL46TextureManager::GetWidth(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Width;
	}

	void OpenGL46TextureManager::SetHeight(TextureHandle handle, uint32_t height)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].Height = height;
	}

	uint32_t OpenGL46TextureManager::GetHeight(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Height;
	}

	void OpenGL46TextureManager::SetTextureType(TextureHandle handle, TextureType textureType)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return;
		m_Textures[handle].Type = textureType;
	}

	TextureType OpenGL46TextureManager::GetTextureType(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return TextureType::DEFAULT;
		return m_Textures[handle].Type;
	}

	uint8_t OpenGL46TextureManager::GetChannels(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return -1;
		return m_Textures[handle].Channels;
	}

	const std::string OpenGL46TextureManager::GetPath(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return std::string();
		return m_Textures[handle].Path;
	}

	TextureData* OpenGL46TextureManager::GetTextureData(TextureHandle handle)
	{
		if (m_Textures.find(handle) == m_Textures.end())
			return nullptr;
		return &m_Textures[handle];
	}
}
