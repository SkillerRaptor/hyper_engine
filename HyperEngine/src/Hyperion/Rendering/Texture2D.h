#pragma once

#include "hppch.h"

namespace Hyperion
{
	enum class TextureType
	{
		DEFAULT,
		FRAME,
		DIFFUSE,
		SPECULAR,
		NORMAL,
		HEIGHT
	};

	class Texture2D
	{
	private:
		unsigned int m_TextureID;
		TextureType m_TextureType;

		int m_Width;
		int m_Height;
		int m_Channels;

	public:
		Texture2D();
		~Texture2D();

		bool LoadTexture(TextureType textureType, const char* filePath, bool alpha = false);
		void GenerateTexture(unsigned char* textureData, bool alpha);

		void BindTexture(unsigned int textureIndex) const;

		void SetTextureType(TextureType textureType);
		TextureType GetTextureType() const;

		void SetWidth(unsigned int width);
		int GetWidth() const;

		void SetHeight(unsigned int height);
		int GetHeight() const;

		int GetChannels() const;
		unsigned int GetTextureId() const;
	};
}