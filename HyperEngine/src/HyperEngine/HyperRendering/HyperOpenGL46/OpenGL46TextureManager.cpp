#include "HyperRendering/HyperOpenGL46/OpenGL46TextureManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HyperCore/Core.hpp"

namespace HyperRendering
{
	OpenGL46TextureManager::OpenGL46TextureManager()
		: m_TextureStorage{ 256 }
	{
		HP_CORE_DEBUG("OpenGL 4.6 Texture-Manager loaded...");
	}

	OpenGL46TextureManager::~OpenGL46TextureManager()
	{
		for (size_t i = 0; i < m_TextureStorage.GetPoolSize(); i++)
			glDeleteTextures(1, &m_TextureStorage[i].Id);
		m_TextureStorage.Clear();
		
		HP_CORE_DEBUG("OpenGL 4.6 Texture-Manager unloaded...");
	}

	TextureHandle OpenGL46TextureManager::Create(const std::string& path, TextureType type)
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
		glCreateTextures(GL_TEXTURE_2D, 1, &id);
		
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		switch (type)
		{
			case TextureType::COMPUTE:
				HP_CORE_WARN("Compute texture type is (yet) not supported in OpenGL 4.6!");
				break;
			case TextureType::DEFAULT:
				glTextureStorage2D(id, 1, channels >= 4 ? GL_RGBA8 : GL_RGB8, width, height);
				glTextureSubImage2D(id, 0, 0, 0, width, height, channels >= 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
				break;
			case TextureType::DIFFUSE:
				HP_CORE_WARN("Diffuse texture type is (yet) not supported in OpenGL 4.6!");
				break;
			case TextureType::COLOR:
				glTextureStorage2D(id, 1, GL_RGB8, width, height);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
				break;
			case TextureType::DEPTH:
				HP_CORE_WARN("Depth texture type is (yet) not supported in OpenGL 4.6!");
				break;
			case TextureType::DEPTH_STENCIL:
				glTextureStorage2D(id, 1, GL_DEPTH24_STENCIL8, width, height);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
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

	TextureHandle OpenGL46TextureManager::Create(uint32_t width, uint32_t height, TextureType type)
	{
		uint32_t id;
		glCreateTextures(GL_TEXTURE_2D, 1, &id);
		
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		switch (type)
		{
			case TextureType::COMPUTE:
				HP_CORE_WARN("Compute texture type is (yet) not supported in OpenGL 4.6!");
				break;
			case TextureType::DEFAULT:
				glTextureStorage2D(id, 1, GL_RGBA8, width, height);
				break;
			case TextureType::DIFFUSE:
				HP_CORE_WARN("Diffuse texture type is (yet) not supported in OpenGL 4.6!");
				break;
			case TextureType::COLOR:
				glTextureStorage2D(id, 1, GL_RGB8, width, height);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
				break;
			case TextureType::DEPTH:
				HP_CORE_WARN("Depth texture type is (yet) not supported in OpenGL 4.6!");
				break;
			case TextureType::DEPTH_STENCIL:
				glTextureStorage2D(id, 1, GL_DEPTH24_STENCIL8, width, height);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
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
	
	void OpenGL46TextureManager::Delete(TextureHandle handle)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		glDeleteTextures(1, &textureData.Id);
		m_TextureStorage.Deallocate(handle.GetIndex());
	}
	
	void OpenGL46TextureManager::Bind(TextureHandle handle, uint32_t textureSlot)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		glBindTextureUnit(textureSlot, textureData.Id);
	}

	void OpenGL46TextureManager::SetWidth(TextureHandle handle, uint32_t width)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		textureData.Width = width;
	}

	std::optional<uint32_t> OpenGL46TextureManager::GetWidth(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Width;
	}

	void OpenGL46TextureManager::SetHeight(TextureHandle handle, uint32_t height)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		textureData.Height = height;
	}

	std::optional<uint32_t> OpenGL46TextureManager::GetHeight(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Height;
	}

	void OpenGL46TextureManager::SetType(TextureHandle handle, TextureType textureType)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;
		textureData.Type = textureType;
	}

	std::optional<TextureType> OpenGL46TextureManager::GetType(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Type;
	}

	void OpenGL46TextureManager::SetData(TextureHandle handle, void* pixels, uint32_t size)
	{
		TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return;

		HP_ASSERT(size == textureData.Width * textureData.Height * textureData.Channels, "Data has to be the same size as the texture!");
		glTextureSubImage2D(textureData.Id, 0, 0, 0, textureData.Width, textureData.Height, textureData.Channels >= 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}

	std::optional<uint8_t> OpenGL46TextureManager::GetChannels(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return std::nullopt;
		return textureData.Channels;
	}

	void* OpenGL46TextureManager::GetImageId(TextureHandle handle) const
	{
		const TextureData& textureData = m_TextureStorage[handle.GetIndex()];
		if (textureData.MagicNumber != handle.GetVersion())
			return nullptr;
		return (void*)static_cast<uintptr_t>(textureData.Id);
	}
}
