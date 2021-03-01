#include "HyperRendering/HyperOpenGL33/OpenGL33TextureManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperRendering
{
	OpenGL33TextureManager::OpenGL33TextureManager()
		: m_TextureStorage{ 256 }
	{
		HP_CORE_DEBUG("OpenGL 3.3 Texture-Manager loaded...");
	}

	OpenGL33TextureManager::~OpenGL33TextureManager()
	{
		for (size_t i = 0; i < m_TextureStorage.GetPoolSize(); i++)
			glDeleteTextures(1, &m_TextureStorage[i].Id);
		m_TextureStorage.Clear();
		
		HP_CORE_DEBUG("OpenGL 3.3 Texture-Manager unloaded...");
	}

	TextureHandle OpenGL33TextureManager::Create(const std::string& path, TextureType type)
	{
		int32_t width;
		int32_t height;
		int32_t channels;
		
		uint8_t* pixels = LoadImage(path, width, height, channels);
		if (pixels == nullptr)
		{
			HP_CORE_ERROR("Failed to load texture!");
			return TextureHandle{ 0 };
		}
		
		uint32_t id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		switch (type)
		{
			case TextureType::COMPUTE:
				HP_CORE_WARN("Compute texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::DEFAULT:
				glTexImage2D(GL_TEXTURE_2D, 0, channels >= 4 ? GL_RGBA : GL_RGB, width, height, 0, channels >= 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
				break;
			case TextureType::DIFFUSE:
				HP_CORE_WARN("Diffuse texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::COLOR:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
				break;
			case TextureType::DEPTH:
				HP_CORE_WARN("Depth texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::DEPTH_STENCIL:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
				break;
			case TextureType::HEIGHT:
				HP_CORE_WARN("Height texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::NORMAL:
				HP_CORE_WARN("Normal texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::SPECULAR:
				HP_CORE_WARN("Specular texture type is (yet) not supported in OpenGL 3.3!");
				break;
			default:
				break;
		}

		FreeImage(pixels);
		
		uint32_t index{};
		TextureData& textureData = m_TextureStorage.Allocate(index);
		textureData.MagicNumber = m_TextureVersion++;
		textureData.Width = width;
		textureData.Height = height;
		textureData.Channels = channels;
		textureData.Type = type;
		textureData.Id = id;
		
		return TextureHandle{ (textureData.MagicNumber << 16) | index };
	}

	TextureHandle OpenGL33TextureManager::Create(uint32_t width, uint32_t height, TextureType type)
	{
		uint32_t id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		switch (type)
		{
			case TextureType::COMPUTE:
				HP_CORE_WARN("Compute texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::DEFAULT:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				break;
			case TextureType::DIFFUSE:
				HP_CORE_WARN("Diffuse texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::COLOR:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
				break;
			case TextureType::DEPTH:
				HP_CORE_WARN("Depth texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::DEPTH_STENCIL:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
				break;
			case TextureType::HEIGHT:
				HP_CORE_WARN("Height texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::NORMAL:
				HP_CORE_WARN("Normal texture type is (yet) not supported in OpenGL 3.3!");
				break;
			case TextureType::SPECULAR:
				HP_CORE_WARN("Specular texture type is (yet) not supported in OpenGL 3.3!");
				break;
			default:
				break;
		}
		
		uint32_t index{};
		TextureData& textureData = m_TextureStorage.Allocate(index);
		textureData.MagicNumber = m_TextureVersion++;
		textureData.Width = width;
		textureData.Height = height;
		textureData.Channels = 4;
		textureData.Type = type;
		textureData.Id = id;
		
		return TextureHandle{ (textureData.MagicNumber << 16) | index };
	}
	
	void OpenGL33TextureManager::Delete(TextureHandle handle)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		glDeleteTextures(1, &textureData.Id);
		m_TextureStorage.Deallocate(handle.GetIndex());
	}

	void OpenGL33TextureManager::Bind(TextureHandle handle, uint32_t textureSlot)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, textureData.Id);
	}

	void OpenGL33TextureManager::SetWidth(TextureHandle handle, uint32_t width)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		textureData.Width = width;
	}

	std::optional<uint32_t> OpenGL33TextureManager::GetWidth(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Width;
	}

	void OpenGL33TextureManager::SetHeight(TextureHandle handle, uint32_t height)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		textureData.Height = height;
	}

	std::optional<uint32_t> OpenGL33TextureManager::GetHeight(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Height;
	}

	void OpenGL33TextureManager::SetType(TextureHandle handle, TextureType type)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		textureData.Type = type;
	}

	std::optional<TextureType> OpenGL33TextureManager::GetType(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Type;
	}

	void OpenGL33TextureManager::SetData(TextureHandle handle, void* pixels, uint32_t size)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		
		HP_ASSERT(size == (textureData.Width * textureData.Height * textureData.Channels), "Data has to be the same size as the texture!");
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureData.Width, textureData.Height, textureData.Channels >= 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}

	std::optional<uint8_t> OpenGL33TextureManager::GetChannels(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Channels;
	}

	void* OpenGL33TextureManager::GetImageId(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return nullptr;
		return (void*)static_cast<uintptr_t>(textureData.Id);
	}
}
