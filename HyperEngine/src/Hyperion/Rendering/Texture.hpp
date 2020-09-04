#pragma once

#include <iostream>
#include <string>

namespace Hyperion
{
	enum class TextureType
	{
		DEFAULT,
		DIFFUSE,
		FRAMEBUFFER,
		HEIGHT,
		NORMAL,
		SPECULAR
	};

	class Texture
	{
	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint8_t m_Channels = 0;
		unsigned char* m_Data = nullptr;

		std::string m_Path = "";
		TextureType m_TextureType = TextureType::DEFAULT;

	public:
		Texture() = default;
		Texture(const std::string& path, TextureType textureType)
			: m_TextureType(textureType), m_Path(path) {}
		Texture(uint32_t width, uint32_t height, TextureType textureType)
			: m_Width(width), m_Height(height), m_TextureType(textureType) {}

		virtual ~Texture() = default;

		virtual bool LoadTexture() = 0;
		virtual void GenerateTexture(bool alpha = true) = 0;
		virtual void DeleteTexture() = 0;
		virtual void BindTexture(uint32_t textureSlot) const = 0;

		virtual uint32_t GetWidth() const { return m_Width; }
		virtual uint32_t GetHeight() const { return m_Height; }
		virtual uint8_t GetChannels() const { return m_Channels; }
		virtual unsigned char* GetData() const { return m_Data; }
		virtual std::string GetPath() const { return m_Path; }
		virtual TextureType GetTextureType() const { return m_TextureType; }
	};
}